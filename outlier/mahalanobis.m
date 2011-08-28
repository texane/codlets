source '/tmp/o.m';
mu = mean(x);
d = x - repmat(mu, size(x)(1), 1);
m = ones(1, size(d)(1));
for i = [1:size(d)(1)]
  m(i) = d(i,:) * inv(cov(x)) * d(i,:)';
end
[i, j] = sort(m);
[ x(j,:) m'(j) ]
