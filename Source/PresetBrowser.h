#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "RoomCatalog.h"

// A message-thread-only view of the existing semantic room indices. No audio state.
class PresetBrowser final : public juce::Component
{
public:
    PresetBrowser()
    {
        setName("Preset-Browser");
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
        rebuildRows();
    }

    std::function<void(int)> onRoomSelected;
    int getSelectedRoom() const noexcept { return selected; }
    float getScrollOffset() const noexcept { return scroll; }
    bool isExpanded(int category) const { return expanded.at(static_cast<std::size_t>(category)); }

    void syncSelection(int room)
    {
        selected = juce::jlimit(0, spatiallab::roomCount - 1, room);
        revealRoom(selected);
        repaint();
    }

    void revealRoom(int room)
    {
        for (int c = 0; c < 4; ++c)
            if (room >= groups[c].first && room < groups[c].end)
            {
                if (!expanded[c]) { expanded[c] = true; rebuildRows(); }
                for (int i = 0; i < rowCount; ++i)
                    if (rows[i].room == room)
                    {
                        const float top = i * rowHeight;
                        if (top < scroll) scroll = top;
                        else if (top + rowHeight > scroll + visibleHeight())
                            scroll = top + rowHeight - visibleHeight();
                        clampScroll(); repaint(); return;
                    }
            }
    }

    juce::Rectangle<float> roomBounds(int room) const { return boundsFor(room, -1); }
    juce::Rectangle<float> categoryBounds(int category) const { return boundsFor(-1, category); }
    void resized() override { clampScroll(); }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff090805));
        g.addTransform(juce::AffineTransform::scale(factor()));
        const float height = visibleHeight();
        for (int i = 0; i < rowCount; ++i)
        {
            const auto row = rows[i];
            const float y = i * rowHeight - scroll;
            if (y + rowHeight <= 0 || y >= height) continue;
            const bool header = row.room < 0;
            const bool active = !header && row.room == selected;
            if (active)
            {
                g.setColour(amber);
                g.fillRoundedRectangle(21, y + 1, 397, rowHeight - 2, 2);
            }
            g.setColour(active ? juce::Colour(0xff100b05) : amber.withAlpha(header ? .78f : 1.0f));
            g.setFont(juce::FontOptions(header ? 16.0f : 18.0f, juce::Font::bold));
            if (header)
            {
                juce::Path arrow;
                if (expanded[row.category])
                { arrow.startNewSubPath(5,y+9); arrow.lineTo(13,y+9); arrow.lineTo(9,y+14); }
                else
                { arrow.startNewSubPath(7,y+7); arrow.lineTo(12,y+11); arrow.lineTo(7,y+15); }
                arrow.closeSubPath(); g.fillPath(arrow);
                g.drawText(juce::String::fromUTF8(groups[row.category].name),
                           juce::Rectangle<float>(19,y,399,rowHeight),juce::Justification::centredLeft,false);
            }
            else
                g.drawText((active ? juce::String::fromUTF8("› ") : juce::String())
                           + juce::String::fromUTF8(spatiallab::roomNames[row.room]),
                           juce::Rectangle<float>(28,y,386,rowHeight),juce::Justification::centredLeft,false);
        }
        // Subtle phosphor lines; the front-panel bezel remains entirely in the artwork.
        g.setColour(juce::Colours::black.withAlpha(.10f));
        for (int y = 0; y < height; y += 4) g.drawHorizontalLine(y,0,422);
        if (maxScroll() > 0)
        {
            g.setColour(amber.withAlpha(.13f)); g.fillRoundedRectangle(426,0,5,height,2);
            const float thumbHeight = height * height / (rowCount * rowHeight);
            const float thumbY = scroll / maxScroll() * (height - thumbHeight);
            g.setColour(amber.withAlpha(.7f)); g.fillRoundedRectangle(426,thumbY,5,thumbHeight,2);
        }
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        if (!e.mods.isLeftButtonDown()) return;
        const auto p = e.position / factor();
        if (p.x >= 422 && maxScroll() > 0)
        {
            draggingScroll = true;
            const float height = visibleHeight();
            const float thumbHeight = height * height / (rowCount * rowHeight);
            const float thumbY = scroll / maxScroll() * (height - thumbHeight);
            dragOffset = p.y >= thumbY && p.y <= thumbY + thumbHeight ? p.y - thumbY : thumbHeight * .5f;
            dragTo(p.y); return;
        }
        const int index = static_cast<int>(std::floor((p.y + scroll) / rowHeight));
        if (index < 0 || index >= rowCount) return;
        const auto row = rows[index];
        if (row.room < 0)
        {
            if (e.getNumberOfClicks() > 1) return;
            // Keep the clicked header at its screen position unless content ends sooner.
            expanded[row.category] = !expanded[row.category];
            rebuildRows(); repaint();
        }
        else if (row.room != selected && onRoomSelected) onRoomSelected(row.room);
    }
    void mouseDrag(const juce::MouseEvent& e) override
    { if (draggingScroll) dragTo(e.position.y / factor()); }
    void mouseUp(const juce::MouseEvent&) override { draggingScroll = false; }
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override
    {
        // JUCE reports a conventional wheel notch as about 0.1, not 1.0.
        scroll -= wheel.deltaY * rowHeight * 30;
        clampScroll(); repaint(); // Consume here; never forward wheel input to a knob.
    }

private:
    inline static constexpr auto groups = spatiallab::categories;
    struct Row { int category = 0, room = -1; };
    std::array<Row,30> rows {};
    std::array<bool,4> expanded {{true,true,true,true}};
    int rowCount = 0, selected = -1;
    float scroll = 0, dragOffset = 0;
    bool draggingScroll = false;
    static constexpr float rowHeight = 23;
    const juce::Colour amber {0xffffad19};
    float factor() const { return std::max(.01f,getWidth()/435.0f); }
    float visibleHeight() const { return getHeight()/factor(); }
    float maxScroll() const { return std::max(0.0f,rowCount*rowHeight-visibleHeight()); }
    void clampScroll() { scroll = juce::jlimit(0.0f,maxScroll(),scroll); }
    void rebuildRows()
    {
        rowCount = 0;
        for (int c = 0; c < 4; ++c)
        {
            rows[rowCount++] = {c,-1};
            if (expanded[c]) for (int room = groups[c].first; room < groups[c].end; ++room)
                rows[rowCount++] = {c,room};
        }
        clampScroll();
    }
    void dragTo(float y)
    {
        const float height = visibleHeight();
        const float thumbHeight = height * height / (rowCount * rowHeight);
        scroll = (y-dragOffset) / std::max(1.0f,height-thumbHeight) * maxScroll();
        clampScroll(); repaint();
    }
    juce::Rectangle<float> boundsFor(int room, int category) const
    {
        for (int i = 0; i < rowCount; ++i)
            if ((room >= 0 && rows[i].room == room)
                || (category >= 0 && rows[i].room < 0 && rows[i].category == category))
                return {0,(i*rowHeight-scroll)*factor(),420*factor(),rowHeight*factor()};
        return {};
    }
};
