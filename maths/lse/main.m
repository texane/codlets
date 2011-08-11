# least square error method
# http://ceee.rice.edu/Books/LA/leastsq/index.html

data = dlmread("dat.csv", " ");
# data = dlmread("parabola.csv", " ");

x = data(:,1);
X = [ones(size(data)(1), 1)'];

degree = 4;
for d = 1:degree
  X = [ X; (x.^d)'];
end
X = X';

X

Y = data(:,2);

# solve the normal eq: X' * X * c = X' * y
c = (X' * X) \ (X' * Y);

# print the resulting linear equation
# printf("x^2 * %f x * %f + %f", c(3), c(2), c(1));

# plot results
y = c(1);
for d = 1:degree
  y = y + x.^d * c(d + 1);
end

plot(x, Y, '.', x, y);
waitforbuttonpress;