This project: We have made a customizable fractal tree on a reflective undulating lake in modern OpenGL.

To run: open terminal in folder and type "make" then "./tree"
(note: it was primarily developed on windows. it should work on linux but we have been unable to find a linux machine that supports glsl 3.30. if it doesn't work on linux for some reason please try to build it on windows.)

To use:

Mouse to move camera
Click and drag to spin around y axis when not in first person mode

Q - change to/from first person mode

In first person mode:
	W - move forward
	S - move backward
	A - move left
	D - move right

	SPACE - move up
	X     - move down

T - decrease maximum angle of branches from y axis
G - increase maximum angle of branches from y axis
R - decrease minimum angle of branches from y axis
F - increase minimum angle of branches from y axis

Y - decrease variability of angle of branches in xz plane
H - increase variability of angle of branches in xz plane

U - decrease thickness of branches
J - increase thickness of branches

I - decrease number of iterations of fractal
K - increase number of iterations of fractal

O - increase the depth of leaves
L - decrease the depth of leaves

Work done by Eli:
The fractal aspect makes use of L-Systems to generate the model. This was an extension of the binary fractal tree into 3 dimensions with other variation added. Added textures to trunk, branches, and leaves. Contributed to shader integration via converting the model to vertex and index buffers.
ref: https://en.wikipedia.org/wiki/L-system#Example_2:_Fractal_(binary)_tree
	(code influenced by description of algorithm)

Work done by Michael:
A significant amount of time was spent learning the programmable graphics pipeline; for example, we have made use of VBO's, EBO's, FBO's, RBO's, VAO's, space tranformation matrices, projective texture mapping, cube maps (skybox), and fragment and vertex shaders. Created undulating water (non-trivial reflections). Implemented orbiting light source. Integrated model and shaders. Implemented user interaction including first person camera.
ref: https://learnopengl.com
	(code influenced by examples and explanations)

Time Spent on Project: 100 hours