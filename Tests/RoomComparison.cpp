#include "Reference/Issue6/RoomEngine.h"
// Historical Phase-1/Phase-2 voicing proof. Issue-6 production voicing is
// measured separately; its early times intentionally differ from this contract.
#include "Reference/RoomEnginePhase1.h"
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct Frame { float l, r; };
struct Metrics { double rt60, late500, early20to80, energy; };
static void require(bool value, const char* what) { if (!value) throw std::runtime_error(what); }
template<class Engine> std::vector<Frame> impulse(double rate, int room)
{
    Engine engine; engine.prepare(rate, room, 0.5f, 0.5f);
    std::vector<Frame> ir(static_cast<std::size_t>(rate * 3));
    for (std::size_t i = 0; i < ir.size(); ++i)
    {
        const float input = i == 0 ? 0.50118723f : 0; // -6 dB send, wet return at unity
        const auto v = engine.process(input, input);
        require(std::isfinite(v.left) && std::isfinite(v.right), "Nonfinite IR");
        ir[i] = {v.left, v.right};
    }
    require(ir[0].l == 0 && ir[0].r == 0, "Direct signal in wet IR");
    return ir;
}
static Metrics metrics(const std::vector<Frame>& ir, double rate)
{
    std::vector<double> tail(ir.size() + 1, 0);
    for (std::size_t i = ir.size(); i-- > 0;)
        tail[i] = tail[i + 1] + static_cast<double>(ir[i].l) * ir[i].l + static_cast<double>(ir[i].r) * ir[i].r;
    require(tail[0] > 1e-8, "Silent IR");
    const auto crossing = [&](double db)
    {
        const double threshold = tail[0] * std::pow(10.0, db / 10.0);
        for (std::size_t i = 0; i < ir.size(); ++i) if (tail[i] <= threshold) return i / rate;
        throw std::runtime_error("IR not long enough for T20");
    };
    return {3 * (crossing(-25) - crossing(-5)), tail[static_cast<std::size_t>(rate * 0.5)] / tail[0],
        tail[static_cast<std::size_t>(rate * 0.02)] - tail[static_cast<std::size_t>(rate * 0.08)], tail[0]};
}
static void le32(std::ostream& out, std::uint32_t v)
{ for (int n = 0; n < 4; ++n) out.put(static_cast<char>((v >> (8 * n)) & 255)); }
static void le16(std::ostream& out, unsigned v) { out.put(static_cast<char>(v & 255)); out.put(static_cast<char>((v >> 8) & 255)); }
static void wave(const std::filesystem::path& file, const std::vector<Frame>& ir, int rate)
{
    std::ofstream out(file, std::ios::binary); require(out.good(), "Cannot write A/B WAV");
    const auto bytes = static_cast<std::uint32_t>(ir.size() * 8);
    out.write("RIFF",4); le32(out,36 + bytes); out.write("WAVEfmt ",8); le32(out,16);
    le16(out,3); le16(out,2); le32(out,static_cast<std::uint32_t>(rate)); le32(out,static_cast<std::uint32_t>(rate * 8));
    le16(out,8); le16(out,32); out.write("data",4); le32(out,bytes);
    for (const auto& f : ir) for (float value : {f.l,f.r})
    { std::uint32_t bits; std::memcpy(&bits,&value,4); le32(out,bits); }
    require(out.good(), "A/B WAV write failed");
}
int main(int argc, char** argv)
{
    try
    {
        if (argc > 1) std::filesystem::create_directories(argv[1]);
        std::cout << "rate,room,old_nominal_rt60,new_nominal_rt60,old_T20_RT60,new_T20_RT60,old_energy_after_500ms,new_energy_after_500ms,early_20_80ms_ratio\n";
        for (double rate : {44100.0,48000.0}) for (int room = 0; room < 3; ++room)
        {
            const auto old = impulse<spatiallab_phase1::Engine>(rate,room), proposed = impulse<spatiallab_issue6_baseline::Engine>(rate,room);
            const auto a = metrics(old,rate), b = metrics(proposed,rate);
            require(b.rt60 > 0.08 && b.rt60 < 0.65, "New neutral room decay outside compact-room range");
            require(b.late500 < 0.002, "Excessive long tail in neutral proposed room");
            if (room > 0) require(b.rt60 < a.rt60 * 0.8 && b.late500 < a.late500 * 0.2, "Kitchen/bath tail not sufficiently shortened");
            if (room == 0) require(b.early20to80 > a.early20to80 * 1.1, "Living-room spatial early field not strengthened");
            std::cout << rate << ',' << room << ',' << spatiallab_phase1::rooms[static_cast<std::size_t>(room)].rt60 << ','
                      << spatiallab_issue6_baseline::rooms[static_cast<std::size_t>(room)].rt60 << ',' << a.rt60 << ',' << b.rt60 << ','
                      << a.late500 << ',' << b.late500 << ',' << b.early20to80 / a.early20to80 << '\n';
            if (argc > 1)
            {
                const auto stem = std::to_string(static_cast<int>(rate)) + "-room-" + std::to_string(room);
                wave(std::filesystem::path(argv[1]) / (stem + "-phase1.wav"),old,static_cast<int>(rate));
                wave(std::filesystem::path(argv[1]) / (stem + "-phase2.wav"),proposed,static_cast<int>(rate));
            }
        }
    }
    catch (const std::exception& e) { std::cerr << e.what() << '\n'; return 1; }
}
