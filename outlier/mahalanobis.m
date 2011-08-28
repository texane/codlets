source '/tmp/o.m';
mu = mean(x);
invcov = inv(cov(x));
d = (x - repmat(mu, size(x)(1), 1))';
m = ones(1, size(d)(2));
for i = [1:size(d)(2)]
  m(i) = d(:,i)' * invcov * d(:,i);
end
% [i, j] = sort(m);
% [ x(j,:) m'(j) ]
