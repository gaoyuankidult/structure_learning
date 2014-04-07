function paraEstimate()

test_data = load('test_data');

connection = load('connection.mat');

p = ones(size(test_data,1),1);

for i = 1:size(test_data,1),
    for j = 1:26,
		p(i) = p(i) * probability(j,connection,test_data);
	end;
end;
