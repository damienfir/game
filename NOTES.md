# Why reimplement functionality manually here ?

1. It is fun
2. I learn a lot
3. It decreases compilation times since I don't compile anything I don't need. Short compilation time decreases
   iteration time which encourages exploration of ideas and debugging. It may not be significant for a small project but
   it will be once the project grows bigger.

# Why use only tetrahedron and octahedron ?

Because they are two primitive shapes that can tessellate when combined. A cube can do that too, but it is boring and
overdone. Also, since I am a programmer and a lousy designer, it allows me to build interesting objects easily.

# Why data-oriented ?

1. It is much easier to reason about a program if we think of how algorithms manipulate data. When using OOP, we have to
   think about behaviors (which behavior gets called when this happens ?). It is also much easier to visualize what is
   happening by printing the data in some way.

# Progress notes

2021-09-27:
Consider using just the terrain heightmap to guide the user vertical position. Too much to collision all the time.
Otherwise the user might slip on slanted surfaces. Actually if we do that we can't just go on another object, for
example a cube on the ground. It is an issue if we want to teleport.
