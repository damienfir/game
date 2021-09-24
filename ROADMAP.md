Graphics
- Lights
- Noisy Phong rendering for visual clues
- Materials

Physics
- Walk on terrain
  - Use collision for walking or just force the player above the ground ?

Mesh
- Load simple mesh and display

Engine
- FPS counter on screen
- Separate the player and the camera
- Separate the functions that manage the world state. Like adding stuff and deleting.
  - They can be made "pure" in the sense that they affect the world only (no input or display).

Geometry
- Use spatial transformation to add octa/tetra to existing face instead of recreating a whole new object

Game
- Make a small world
  - Even generate automatically, like a floor

Editor
- Move selected objects as whole
  - Selected objects are groups of polyhedra
- Change color of object/polyhedra
- Do not allow overlapping with other objects
- Camera "snaps" to faces when not turning around
- Rotate octahedron

Debugging
- Visualize bounding boxes
- 3rd person view of the camera

Testing
- Start to write tests for pure functions
- Move more functions to be pure so they can be tested
  - Physics
  - Maths
  - Undo/redo
  - Mesh/geometry

Blogging
- Undo/redo system with std::variant and Actions