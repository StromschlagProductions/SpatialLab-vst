param([string]$Destination)
Add-Type -AssemblyName System.Speech
$folder = [IO.Path]::GetFullPath($Destination)
[IO.Directory]::CreateDirectory($folder) | Out-Null
$voice = New-Object System.Speech.Synthesis.SpeechSynthesizer
$voice.Volume = 60
$voice.Rate = 0
$text = "Behind the heavy door, the house was quiet. I put the keys on the table and listened. A glass touched the sink. Then a soft voice came from the next room: leave the light on, I will be back before morning."
$format = [System.Speech.AudioFormat.SpeechAudioFormatInfo]::new(48000, [System.Speech.AudioFormat.AudioBitsPerSample]::Sixteen, [System.Speech.AudioFormat.AudioChannel]::Mono)
$voice.SetOutputToWaveFile("$folder/speech-source.wav", $format)
$voice.Speak($text)
$metadata = @{text=$text;voice=$voice.Voice.Name;culture=$voice.Voice.Culture.Name;volume=60;rate=0;sampleRate=48000;channels=1;bits=16;origin="New original text synthesized locally by Windows System.Speech; no third-party recording"}
$voice.Dispose()
$metadata | ConvertTo-Json | Out-File "$folder/source.json" -Encoding utf8
