# Python3 program for implementing 
# Mid-PoCircle Drawing Algorithm 

# Implementing Mid-PoCircle Drawing 
# Algorithm 
def midPointCircleDraw(center, r): 

	points = []
	x = r 
	y = 0
	x_centre = center[0]
	y_centre = center[1]

	# Printing the initial poon the 
	# axes after translation 
	points.append([r + x_centre, y_centre])
	
	# When radius is zero only a single 
	# powill be printed 
	if (r > 0) : 
		points.append([-r + x_centre, y_centre])
		points.append([x_centre, r + y_centre])
		points.append([x_centre, -r + y_centre])
	
	# Initialising the value of P 
	P = 1 - r 
	while (x > y) : 
	
		y += 1
		
		# Mid-pois inside or on the perimeter 
		if (P <= 0): 
			P = P + 2 * y + 1
			
		# Mid-pois outside the perimeter 
		else:		 
			x -= 1
			P = P + 2 * y - 2 * x + 1
		
		# All the perimeter points have already been printed 
		if (x < y): 
			break
		
		# Printing the generated poand its reflection in the other octants after translation 
		points.append([x + x_centre, y + y_centre])
		points.append([-x + x_centre, y + y_centre])
		points.append([x + x_centre, -y + y_centre])
		points.append([-x + x_centre, -y + y_centre])
		
		# If the generated pois on the line x = y then the perimeter points have already been printed 
		if (x != y) : 
			points.append([y + x_centre, x + y_centre])
			points.append([-y + x_centre, x + y_centre])
			points.append([y + x_centre, -x + y_centre])
			points.append([-y + x_centre, -x + y_centre])
	
	return points
							
# Driver Code 
if __name__ == '__main__': 
	for px, py in product(range(11), range(11)):
		print(x, y)
	# To draw a circle of radius 3 
	# centred at (0, 0) 
	#midPointCircleDraw((0, 0), 5) 

# This code is contributed by 
# SHUBHAMSINGH10 
