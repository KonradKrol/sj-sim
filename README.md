# Sj.Sim

Sj.Sim is a ski jumping results simulator with a graphical interface built using Qt Widgets (C++).

The project was mainly developed between January and November 2023.

It focuses on simulation, not graphics — there is no 2D or 3D rendering. Instead, results are based on a probability system and modeled athlete skills.

## Main modes

- **Simulate season**  
  A full season mode where the game goes through a generated calendar of competitions.  
  Includes features to improve realism:
  - automatic form system for athletes  
  - charts and statistics  
  - result manipulation tools  
  - ability to change the database during the season  

- **Single competition**  
  Run one competition on a selected hill with a chosen start list.

- **Single jumps**  
  Lets you set up conditions (hill, athlete, wind) and simulate multiple jumps.  
  Shows detailed data, charts, and overall statistics.

- **Database editor**  
  Edit the global list of athletes, hills, and competition rules.

## Features
- Custom databases (you can load your own jumpers and setups)  
- Multi-season gameplay (if God permits and it won't crash...)
- Simulation based on probability and athlete skill models  
- Advanced statistics and charts  

## Technical notes
- Built with Qt Widgets and C++  
- Codebase is hard to maintain  
- Contains many bugs, memory leaks, and outdated patterns  
- Debugging was often replaced by print-based approaches  

## Building on Windows

Install Qt 6.7.3 for MSVC 2022 x64 with Qt Charts, then open an x64 Native
Tools Command Prompt for VS 2022. From the repository root:

```bat
mkdir build-win
cd build-win
qmake ..\sj-sim.pro CONFIG+=release
nmake
cd ..
powershell -ExecutionPolicy Bypass -File scripts\package-windows.ps1 -BuildDir build-win -OutputDir dist\sj-sim-windows-x64
```

Run `dist\sj-sim-windows-x64\sj-sim.exe`. Qt is dynamically linked, so the
package contains the executable and the required runtime DLLs. Compiler object
files stay in the build directory and are never included. The current portable
build uses the bundled DPP compatibility layer, so Discord webhook delivery is
disabled and no external DPP package is required.

## Automated releases

Every commit pushed to `main` builds Windows and Linux packages and publishes
them as a prerelease in the GitHub Releases tab. The same workflow can be run
on demand from the Actions tab. Each downloadable archive contains only runtime
files and places `flags`, `translations`, and `userData` beside the executable.

This project should be treated as experimental and educational rather than production-quality.

## Known issues

There is a well-known crash in season mode.  
Fix/workaround: https://www.youtube.com/watch?v=coQ_zKPa1xk

## Remake

A modern remake has been developed (React and Electron):
https://github.com/KonradKrol/sjsim-2026  

The new version aims to clean up the architecture and fix major issues.

## Contributing

This project is very open to contributions.  
If something is unclear or hard to understand, feel free to ask — I can walk you through the code and explain key parts.

Even small improvements help to clean up* the project.

## Screenshots
![image (1)](https://github.com/user-attachments/assets/25282487-6e3f-4038-8711-4a08344059c9)
![image (2)](https://github.com/user-attachments/assets/8a9c473e-140d-4551-9f4e-eb7b1a2bec48)
![image (3)](https://github.com/user-attachments/assets/bf90b645-8b65-4667-8e16-517ea76c6513)
![image (4)](https://github.com/user-attachments/assets/029dcf2b-e0c1-4c8d-8d50-df91dc7340db)
![image (5)](https://github.com/user-attachments/assets/5fcf56f1-6574-4323-a39f-e2282b08a1cf)
![image (6)](https://github.com/user-attachments/assets/95dc3db4-b961-4526-8b4f-dad94bd0d00a)
![image (7)](https://github.com/user-attachments/assets/fd02a569-7481-4f9c-894f-5f4454a05720)
![image (8)](https://github.com/user-attachments/assets/a9b8e351-dbe9-4fb8-b9fb-ba274946a9ba)
![image (9)](https://github.com/user-attachments/assets/9ee5446a-57c0-49a9-ac7b-3b9e503743e6)
![image (10)](https://github.com/user-attachments/assets/29498a9d-00c8-4e4f-859e-a1842b98b259)
![image (11)](https://github.com/user-attachments/assets/75b4637e-a06c-4bcd-bad1-2b9c21a9dc88)
![image (12)](https://github.com/user-attachments/assets/b44b3b9c-ac08-42cb-b245-1fb98a3178e3)
![image (13)](https://github.com/user-attachments/assets/17c49b25-fd4d-45c4-948f-e96926b0caa2)
![image (14)](https://github.com/user-attachments/assets/e7e861df-85b6-49ec-8630-e85e6840a99e)
![image (15)](https://github.com/user-attachments/assets/bd460cb4-e411-400f-8e6e-45adae52d8e8)
![image (16)](https://github.com/user-attachments/assets/ddac3406-42fa-4ec6-8806-a85aa0e9bd4d)
![image (17)](https://github.com/user-attachments/assets/097b0d11-bb81-47f1-b942-028350691a63)
![image (18)](https://github.com/user-attachments/assets/617e6f99-59e7-42c3-b3c1-d8da7c670542)
![image (19)](https://github.com/user-attachments/assets/563be031-2d5c-4ba6-aed2-0147e5d74805)
![image (20)](https://github.com/user-attachments/assets/ffdcd959-7857-454d-acca-ec92b99bc517)
![image (21)](https://github.com/user-attachments/assets/6ab3be21-4abf-42c6-baec-d694aec60419)
![image (22)](https://github.com/user-attachments/assets/2b297f2b-cb9f-49a8-8edc-7c609991b445)
![image (23)](https://github.com/user-attachments/assets/d144cf23-3a47-421b-b683-31c53890875d)
![image (24)](https://github.com/user-attachments/assets/3d012fc2-1aff-47d3-b1d4-56f44388291a)












