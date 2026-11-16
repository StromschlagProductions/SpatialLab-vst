#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <cmath>
#include <iostream>
#include <stdexcept>
void check(bool b,const char* s){if(!b)throw std::runtime_error(s);}
void write(const juce::File& file,const juce::AudioBuffer<float>& audio,double rate)
{
    check(audio.getMagnitude(0,audio.getNumSamples())<0.9999f,"Clipping: abort rather than normalize");
    juce::WavAudioFormat format;
    auto stream=file.createOutputStream();check(stream!=nullptr,"Output unavailable");
    std::unique_ptr<juce::AudioFormatWriter> writer(format.createWriterFor(stream.release(),rate,static_cast<unsigned>(audio.getNumChannels()),16,{},0));
    check(writer!=nullptr && writer->writeFromAudioSampleBuffer(audio,0,audio.getNumSamples()),"WAV write failed");
}
int main(int argc,char** argv)
{
    juce::ScopedJuceInitialiser_GUI init;
    try {
        check(argc==4,"Usage: renderer bundle speech.wav output");
        juce::File bundle=juce::File::getCurrentWorkingDirectory().getChildFile(argv[1]);
        juce::File speech=juce::File::getCurrentWorkingDirectory().getChildFile(argv[2]);
        juce::File out=juce::File::getCurrentWorkingDirectory().getChildFile(argv[3]);out.createDirectory();
        juce::AudioPluginFormatManager manager;manager.addDefaultFormats();
        juce::OwnedArray<juce::PluginDescription> descriptions;
        for(auto* format:manager.getFormats())format->findAllTypesForFile(descriptions,bundle.getFullPathName());
        check(descriptions.size()==1,"Expected exactly one built VST3 audio processor");
        check(descriptions[0]->manufacturerName=="Stromschlag Audio","Wrong actual module manufacturer");
        juce::AudioFormatManager readers;readers.registerBasicFormats();
        std::unique_ptr<juce::AudioFormatReader> reader(readers.createReaderFor(speech));check(reader!=nullptr,"Speech source missing");
        check(reader->sampleRate==48000 && reader->numChannels==1,"Expected 48k mono speech");
        check(speech.copyFileTo(out.getChildFile("speech-dry.wav")),"Dry source copy failed");
        std::cout<<"source,room,rate,wet_peak,mix_peak\n";
        for(int source=0;source<2;++source)
        {
            const double rate=source==0?48000:44100;
            const int active=source==0?static_cast<int>(reader->lengthInSamples):static_cast<int>(rate*9);
            const int length=active+static_cast<int>(rate*3);
            juce::AudioBuffer<float> dry(2,length);dry.clear();
            if(source==0){reader->read(&dry,0,active,0,true,false);dry.copyFrom(1,0,dry,0,0,length);}
            else for(int n=0;n<active;++n){const double t=n/rate;const double env=std::min(1.0,t/.05)*std::min(1.0,(9-t)/.1);const float x=static_cast<float>(env*(.16*std::sin(t*2*juce::MathConstants<double>::pi*45)+.08*std::sin(t*2*juce::MathConstants<double>::pi*80)+.04*std::sin(t*2*juce::MathConstants<double>::pi*160)));dry.setSample(0,n,x);dry.setSample(1,n,x);}
            const juce::String stem=source==0?"speech":"bass";
            if(source==1)write(out.getChildFile(stem+"-dry.wav"),dry,rate);
            for(int room=0;room<3;++room)
            {
                juce::String error;auto plugin=manager.createPluginInstance(*descriptions[0],rate,256,error);check(plugin!=nullptr,"Built VST3 failed to instantiate");
                juce::AudioProcessorParameter *roomParameter=nullptr,*sizeParameter=nullptr,*toneParameter=nullptr;
                for(auto* p:plugin->getParameters())
                {
                    const auto name=p->getName(100);
                    if(name=="Raum")roomParameter=p;
                    if(name=="Klang")toneParameter=p;
                    if(name.startsWith("Gr"))sizeParameter=p;
                }
                check(roomParameter && sizeParameter && toneParameter,"Actual module room/size/tone missing");
                if(auto* bypass=plugin->getBypassParameter()) bypass->setValueNotifyingHost(0);
                roomParameter->setValueNotifyingHost(room/2.0f);
                sizeParameter->setValueNotifyingHost(.5f);toneParameter->setValueNotifyingHost(.5f);
                check(std::abs(roomParameter->getValue()-room/2.0f)<.001f && sizeParameter->getValue()==.5f && toneParameter->getValue()==.5f,"Actual VST3 parameter values not applied");
                plugin->setPlayConfigDetails(2,2,rate,256);plugin->prepareToPlay(rate,256);
                juce::AudioBuffer<float> wet(2,length),mix(2,length);wet.clear();
                juce::MidiBuffer midi;constexpr float send=0.5011872336f;
                for(int offset=0;offset<length;offset+=256){int count=std::min(256,length-offset);juce::AudioBuffer<float> block(2,count);for(int c=0;c<2;++c)block.copyFrom(c,0,dry.getReadPointer(c,offset),count,send);plugin->processBlock(block,midi);for(int c=0;c<2;++c)wet.copyFrom(c,offset,block,c,0,count);}
                mix.makeCopyOf(dry);for(int c=0;c<2;++c)mix.addFrom(c,0,wet,c,0,length);
                check(wet.getMagnitude(0,1)==0,"Direct signal at wet sample zero");
                for(int c=0;c<2;++c)for(int n=0;n<length;++n)check(std::isfinite(wet.getSample(c,n)),"Nonfinite output");
                write(out.getChildFile(stem+"-room-"+juce::String(room)+"-wet.wav"),wet,rate);
                write(out.getChildFile(stem+"-room-"+juce::String(room)+"-mix.wav"),mix,rate);
                std::cout<<stem<<','<<room<<','<<rate<<','<<wet.getMagnitude(0,length)<<','<<mix.getMagnitude(0,length)<<"\n";
                plugin->releaseResources();
            }
        }
    }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}
}
