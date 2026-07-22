# Rescue the Princess

## Story / gameplay
1. **Intro:** the villain climbs the slope carrying the princess to the temple at the top.
2. **Playing:** you control the knight, climbing the same slope to catch up. The
   villain (now at the top) throws rolling stones and fires arrows at you.
   - Jumping dodges stones, but NOT arrows.
   - Getting hit costs 1 of your 3 lives, then you're briefly invulnerable
     (the knight flashes) so you don't lose multiple lives to one attack.
3. **Win:** reach the top (t = 100%) to rescue the princess.
4. **Lose:** run out of lives.

## Controls
- **Up / W** — climb the slope
- **Down / S** — descend
- **Space** — jump
- **R** — restart after Win/Game Over
- **Esc** — quit

## Classes (all in separate .hpp/.cpp files)
- `GameObject` — abstract base class (sprite + texture + update/draw), everything below inherits from it
- `Knight` — hero: movement, jumping, 3 lives, invulnerability window
- `Villain` — climbs during the intro, then throws `RollingStone`/`Arrow` on timers
- `Princess` — passive, just follows the villain's position
- `Projectile` (abstract, in `Projectile.hpp`) — base for `RollingStone` and `Arrow`
- `Game` — owns everything, runs the main loop, handles state (Intro/Playing/Won/GameOver)
- `Utility::loadTextureOrPlaceholder()` — loads your art, or a colored square if a file isn't there yet (so the game runs even before all art is ready)

## Art you've already given me (already placed in `assets/`)
- `assets/background.png` ✅
- `assets/knight.png` ✅
- `assets/princess.png` ✅

## Art still needed — use these EXACT filenames
Save each drawing directly into the `assets` folder with this exact name:

- **`assets/villain.png`** — the villain/kidnapper character
- **`assets/stone.png`** — a single rolling boulder/rock (drawn as a roughly
  square image; it'll be scaled down and rotated automatically as it rolls)
- **`assets/arrow.png`** — a single arrow, drawn pointing to the RIGHT
  (→). This matters: the code rotates it to match its flight direction,
  and assumes "pointing right" is the arrow's neutral/default pose.

Until you add these, the game still runs — it just shows a plain colored
square in place of whichever image is missing, so you can test the
gameplay right away.

## Build (using your matching SFML + mingw64 compiler)

1. Put all the `.hpp`/`.cpp` files and the `assets` folder together in one project folder.

2. Copy the SFML DLLs:
   ```
   copy "E:\CPP_Workspace\SFML-2.6.1\bin\*.dll" .
   ```

3. Copy the matching compiler runtime DLLs (must be from the SAME compiler you compile with):
   ```
   copy "<path-to-your-matching-mingw64>\bin\libgcc_s_seh-1.dll" .
   copy "<path-to-your-matching-mingw64>\bin\libstdc++-6.dll" .
   copy "<path-to-your-matching-mingw64>\bin\libwinpthread-1.dll" .
   ```

4. Compile:
   ```
   g++ main.cpp Game.cpp Knight.cpp Villain.cpp Princess.cpp -o game.exe -I"E:\CPP_Workspace\SFML-2.6.1\include" -L"E:\CPP_Workspace\SFML-2.6.1\lib" -lsfml-graphics -lsfml-window -lsfml-system
   ```
   (Use the full path to your matching compiler's `g++.exe` instead of just `g++` if it's not the one already on your PATH.)

5. Run:
   ```
   .\game.exe
   ```
