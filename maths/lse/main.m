# least square error method
# http://ceee.rice.edu/Books/LA/leastsq/index.html

data = dlmread("dat.csv", " ");
x = data(:,1);
X = [ones(size(data)(1), 1)'; x' ]';
Y = data(:,2);

# solve the normal eq: X' * X * c = X' * y
c = (X' * X) \ (X' * Y);

# print the resulting linear equation
printf("x * %f + %f", c(2), c(1));

# plot results
plot(x, Y, '.', x * c(2) + c(1));
waitforbuttonpress;