Graphics
- Lights
- Noisy Phong rendering for visual clues
- Materials

Physics
- Walk on terrain
  - Use collision for walking or just force the player above the ground ?
  - Consider using just the terrain heightmap to guide the user vertical position. Too much to collision all the time. Otherwise the user might slip on slanted surfaces.

Mesh
- Load simple mesh and display
  - Handle all generic mesh, not only tetra/octa (too complicated to build anything with that)

Engine
- Separate the player and the camera
- Separate the functions that manage the world state. Like adding stuff and deleting.
  - They can be made "pure" in the sense that they affect the world only (no input or display).

Geometry

Game
- Make a small world
  - Even generate automatically, like a floor

Editor


Debugging
- Visualize bounding boxes
- 3rd person view of the camera
- FPS on screen

Testing
- Start to write tests for pure functions
- Move more functions to be pure so they can be tested
  - Physics
  - Maths
  - Undo/redo
  - Mesh/geometry

Blogging
- Undo/redo system with std::variant and Actions