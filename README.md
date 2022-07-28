# Boids in Unreal Engine 4
Boids is an artificial life program which simulates the flocking behaviour of birds.
As with most artificial life simulations, Boids is an example of emergent behavior; that is, the complexity of Boids arises from the interaction of individual agents (the boids, in this case) adhering to a set of simple rules. The rules applied in the simplest Boids world are as follows:

separation: steer to avoid crowding local flockmates
alignment: steer towards the average heading of local flockmates
cohesion: steer to move towards the average position (center of mass) of local flockmates
		
I've made it so the user can change the alignment, cohesion, avoidance, and sphere radius within UE4 ins the detail section instead of going back to the .cpp file. Having this allowed me to try different force values until the boids behave the way that I wanted to. 
For alignment, I've found that 30 was a decent amount.
For cohesion, I've found that 5 is the perfect amount since more makes the balls hover above each other, and with less, they get lost as the player moves around.
For avoidance, I've set it to 25 since more makes them be to spread out and easily get lost, and with too little they are cramped together making cohesion shoot the balls and behave wierdly.
For sphere radius, I've set it to 750 so the balls are always with the reach of the player-controlled one.

# Video: 
		https://youtu.be/Ku6LRBKVfro
