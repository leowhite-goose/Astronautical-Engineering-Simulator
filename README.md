# Astronautical-Engineering-Simulator
![alt text](https://github.com/leowhite-goose/Astronautical-Engineering-Simulator/blob/main/RSS-System-Reduced-Distance.png?raw=true)
## About
This is only really on Github so I can host a website demo to show off; this is a heavy work in progress and was basically just started (for fun). The idea is that, while I learn cool engineering stuff during my mechanical engineering undergrad (e.g., thermodynamics), I can use this as a study guide of sorts by implementing what I learn, here.

## Acknowledgments
This project uses SDL3 and cglm--both pretty cool projects (AES initially used GL4ES too, but doesn't anymore; that's also a cool project you should check out)

## To-do List
0. Standardize conventions (I have no idea what I'm doing; no, I won't use A.I.)
1. ~~Fix multi-touch attempt~~ (done)
2. ~~Actually load object data into some world space (using 128bit integer coordinates; 1fm blocks out to 3.4 * 10^7 LY)~~
3. Test out Swartzchild n-body gravity (no idea if my implementation is right; use fp32 for math using 128bit coords)
4. Gravity simulation using cells (i.e., allow for galaxy/universe-scale gravity; use with n-body at small scales)
5. ~~Fix web demo (web demo is currently behind, as this program is mixed shaders/FFP)~~
---
## Using the Program
If you just want to run the program, using a machine running some typical desktop GNU Linux distro:
1. Download the entire repo, minus the minus the ".html", ".js", and ".wasm" files in the root directory
2. Run the "AES" file like any other executable file/program (you may need to add permissions with "chmod +x ./AES" from a terminal in the downloaded folder)
3. Load 3D FEM meshes by generating them first with NetGen or Gmsh in FreeCAD, then export them as "FEM FENICS MESH" into AES's "meshes" folder, then change "char model_file[]" to equal its file name
4. For a "demo", see https://leowhite-goose.github.io/Astronautical-Engineering-Simulator/
---
## Compiling and Running the Program (by platform)
### Compiling for GNU Linux:
1. run either one of the following from a terminal to build/compile the code:
    - gcc src/main/main.c -o AES -lSDL3 -lGL -lm
        - requires fedora's "SDL3-devel.x86_64" or your distro's equivalent package
    - gcc src/main/main.c -o AES -I lib/ -lGL -lm
        - using included SDL3 library (lib/SDL3 is empty, so provide your own pre-compiled one for your system there)
        
2. to run the program (or just double click the file generated from above)
    - ./AES

### Compiling for Windows (using Window's Subsystem for Linux; i.e., a lazy Windows port)
1. download FedoraLinux-44 template for WSL via Windows powershell:
    - WSL --install -d FedoraLinux-44
2. download everything from in the repo minus the ".html", ".js", and ".wasm" files in the root directory
3. move the downloaded folder from above into the WSL machine
4. once in WSL machine run:
    - sudo dnf install SDL3 mesa-libGL &&* chmod +x ./AES
5. run program with:
    - ./AES

### Compiling for Emscripten
1. search "emscripten" and go through the intro steps for setting it up; you'll its commands to run the following:
2. to build:
    Slower (physics runs at 1/100th speed), because this is with ram growth
    - emcc src/main/main.c -o index.js --use-port=sdl3 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=24mb -s MAXIMUM_MEMORY=384mb -s TOTAL_STACK=16mb -DSDL_THREADS-ON -pthread -sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency --embed-file data/icon.png --embed-file meshes/TOS-rip-FEMMeshGmsh002.xml --embed-file meshes/TOS-Enterprise-G14.xml --embed-file meshes/Sphere_Diameter=1.xml --embed-file src/graphics/vertex_shader.glsl --embed-file src/graphics/fragment_shader.glsl --embed-file src/graphics/gui_vertex_shader.glsl --embed-file src/graphics/gui_fragment_shader.glsl --embed-file src/graphics/non_lit_fragment_shader.glsl
    Regular (physics runs ~regularly), because this is without ram growth; crashes a lot
    - emcc src/main/main.c -o index.js --use-port=sdl3 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -s INITIAL_MEMORY=1024mb -s TOTAL_STACK=128mb -DSDL_THREADS-ON -pthread -sPTHREAD_POOL_SIZE=navigator.hardwareConcurrency --embed-file data/icon.png --embed-file meshes/TOS-rip-FEMMeshGmsh002.xml --embed-file meshes/TOS-Enterprise-G14.xml --embed-file meshes/Sphere_Diameter=1.xml --embed-file src/graphics/vertex_shader.glsl --embed-file src/graphics/fragment_shader.glsl --embed-file src/graphics/gui_vertex_shader.glsl --embed-file src/graphics/gui_fragment_shader.glsl --embed-file src/graphics/non_lit_fragment_shader.glsl
        - note: build command below targets "index.js" and leaves "index.html" untouched; target "index.html" to regen all website-related files
3. to run a server to host the generated site:
    - emrun ./index.html                        // starts server & auto-opens a tab w/ default browser

---
## Notes & Contributing (or not)
I just started using Github with this project and am using it to learn programming & mechanical engineering; I probably won't accept pull or merge requests as I don't really know how they work and don't feel like it. Additionally, the project was created entirely without the use of generative A.I. (minor caveat: I didn't know DeepWiki was A.I. generated until recently; I don't remember it being particularly helpful and won't be using it going forward); this is evident in that no half-decent chatbot would write code this ~~poorly~~ "creatively" with purely hypothetical guidelines and consistency (plus the Stack Overflow & other such links scattered throughout this codebase).

### Using Git (because I forget)
Examples of basic commands:
- git add . <-- adds all changes
- git add example.txt
- git rm example.txt
- git commit -m "removed example.txt"
- git status
- git push origin main
