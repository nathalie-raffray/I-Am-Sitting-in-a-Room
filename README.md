# I Am Sitting in a Room

This is a simple console program that automates what Alvin Lucier did to record [I Am Sitting in a Room](https://en.wikipedia.org/wiki/I_Am_Sitting_in_a_Room). It uses [dr_libs](https://github.com/mackron/dr_libs) and [rtaudio](https://github.com/thestk/rtaudio).

Alvin Lucier was this guy who in 1969 recorded himself narrating this [text](https://en.wikipedia.org/wiki/I_Am_Sitting_in_a_Room#Full_text)...

He played that recording into a room, then re-recorded it, then played that re-recording back into the room, then re-re-recorded it, then played that re-re-recording back into the room and so on, until the room's presence in the audio - through the resonance, sound bouncing off whatever architecture was present - became [exaggerated](https://youtu.be/fAxHlLK3Oyk?t=2282). 

The spoken text sort of disintegrated into something unintelligible, amorphous, and underwater-like, something we could deem 'electronic-like', a sort of warping glass-like sound. The sound waves are the moving performers of this music, they orchestrate themselves, and Alvin Lucier was there only to set up the environment for them to move in.

![image](https://user-images.githubusercontent.com/43141187/220886959-da1ac21b-73b1-4e4e-a38d-d36647fd3ac1.png)

His idea of looping sound until its disintegration is not all so different from what William Basinski did with [The Disintegration Loops](https://en.wikipedia.org/wiki/The_Disintegration_Loops) only that was decades later.

Back in the day Lucier did this all with tape of course. I can only imagine how this manual & repetitive process of recording, playing, and editing could be in turns meditative and frustrating. Computers dig loops though.. you give them enough staying power and they can loop forever. They welcome your `while(true)` with great hospitality.

I like the idea of leaving a computer in the middle of a room and letting it run all day recording and playing back audio until its distorted, perhaps ugly conclusion. Like Alvin Lucier did in 1969. So I programmed it. You just need to give as parameter an audio input file, then the program will prompt you for which microphone and speaker you want to use and the number of loops you want. The amalgated output - all the loops - are output to a .wav file at the end.

## Build I-Am-Sitting-in-a-Room

Visual Studio 2019 .sln in projects/vs2019. CMakeLists.txt incoming maybe.

## Run I-Am-Sitting-in-a-Room

Binary is in Releases. For now, only runs on Windows because it uses WASAPI and platform-dependent Unicode console mode. Sucks to suck (sorry my \*nix friends...).

The program expects two parameters, an input .mp3 sound file and an output folder where a generated .wav file will be exported to.

`I-Am-Sitting-in-a-Room-winx64.exe -i [inputMp3Filepath] -o [outputFolder]`


## Skip here if you are lost (non programmey people)

I'll give you some easy steps to make this run.
If you are still lost, please don't hesitate to dm me on instagram @greenmoonyellowmoon, or email me at nathalie.raffray9@gmail.com. I am very friendly, there are no stupid questions.

If you don't have Windows, I am sorry.. this won't work.

### Steps

1. Click on the latest release on the right (`1.0-win64` in pic below).

<img width="204" alt="releases" src="https://user-images.githubusercontent.com/43141187/220881283-bfce8ce4-9dd1-46a3-a8f4-4bceb114a4e7.png">

2. Click on `I-Am-Sitting-in-a-Room-winx64.exe`. It will download automatically.

<img width="422" alt="exe file" src="https://user-images.githubusercontent.com/43141187/220881805-7e70857c-1242-4091-923d-bc3a76d9559d.png">

3. Make a folder somewhere in your File Explorer, and drag the `I-Am-Sitting-in-a-Room-winx64.exe` there and the mp3 audio file you want to use as input.

4. Click inside the address bar of your File Explorer, until it becomes editable as below. You can also click the `v` arrow to the right of the address bar.

<img width="989" alt="click on file path bar" src="https://user-images.githubusercontent.com/43141187/220882942-143320be-a5f0-41a8-b109-0baca6cd33fd.png">

5. Type `cmd` and press enter and a command prompt window will open.

<img width="1002" alt="cmd" src="https://user-images.githubusercontent.com/43141187/220882975-fa12a984-14c1-45fd-a335-3021eebf5e7d.png">

6. Type `I-Am-Sitting-in-a-Room-winx64.exe -i audio.mp3 -o .` in the command prompt. Replace `audio.mp3` with the name of your .mp3 file if it has a different name. Press enter.

<img width="650" alt="in the cmd" src="https://user-images.githubusercontent.com/43141187/220883586-6d0f0d05-46fe-489e-ac25-01e27180b61b.png">

Congrats. Program should be running, follow the instructions. It will output the resulting .wav file directly into your folder. Now you have all the power to make your shitty sound art piece. <3



![this beat sounds](https://user-images.githubusercontent.com/43141187/220887895-8d04ac2f-a2c1-4439-9ba3-994f67fd5af2.jpg)


