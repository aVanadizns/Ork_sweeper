# Ork Sweeper — Minesweeper spēle C++ un SFML3

Ork Sweeper ir klasiskā Minesweeper spēles interpretācija, uzrakstīta C++ valodā, izmantojot SFML 3.0 bibliotēku un paredzēta palaišanai Visual Studio 2022 vidē.

<img width="183" height="293" alt="image" src="https://github.com/user-attachments/assets/1a162ebf-7ea6-454a-aefa-b6cbe131d99f" />   <img width="184" height="291" alt="image" src="https://github.com/user-attachments/assets/fc86f804-f58a-4e8d-82fd-b0e32f32f7d0" />

### Spēlē ir:
- drošais pirmais klikšķis (mīnas ģenerējas tikai pēc pirmā klikšķa),
- automātiska nulles lauciņu atvēršana (BFS algoritms),
- karodziņu sistēma,
- taimeris un mīnu skaitītājs,
- popup logs ar iespēju spēlēt no jauna,
- grafiskais interfeiss, kas balstīts uz sprite sheet un fontiem.

### Prasības un rīki
Lai projektu varētu palaist vai kompilēt, nepieciešams:
1. Visual Studio 2022
Ar sekojošiem komponentiem:
- Izveidots projekts ar Desktop development with C++
- MSVC v143 toolset
- C++ 17 valodas standars
  
2. SFML 3.0 (vai jaunāka versija)
Projekts izmanto šādus SFML moduļus:
- sfml-graphics
- sfml-window
- sfml-system
  
#### Kā pievienot SFML3 projektam:
Lejupielādē jaunāko versiju:
https://www.sfml-dev.org/download/

Pievieno nepieciešamos failus projektam pēc pamācības:
https://www.sfml-dev.org/tutorials/3.0/getting-started/visual-studio/

vai arī pēc video:
https://www.youtube.com/watch?time_continue=130&v=qvg8BXXWpCE&embeds_referring_euri=https%3A%2F%2Fwww.bing.com%2F&embeds_referring_origin=https%3A%2F%2Fwww.bing.com&source_ve_path=Mjg2NjY

### Kā palaist spēli
No Visual Studio:
- Atver .sln failu
- Uzstādi:
  - Debug x64 vai Release x64
- Pārliecinies, ka SFML DLL faili ir iekļauti izpildmapē
- Spied Local Windows Debugger
