%a function for counting the counts of samples and calculating BDeu score for the input variables
function score = bdeuScore2(variable, parents, alpha,training_data)

combination = ones(3^(length(parents)+1),length(parents)+1);
counts = zeros(3^(length(parents)+1),1);

%generate all possible combinations
for i = 1:size(combination,1),
    num = i-1;
    for j = length(parents):-1:0,
        combination(i,length(parents)+1-j) = combination(i,length(parents)+1-j) + floor((num)/(3^j));
        num = mod(num,3^j);
    end;
end;
%{
%only select relevant columns 
data = [];

for i = 1:length(parents),
    data = [data,training_data(:,parents(i))];
end;
%add the value of kid
data = [data,training_data(:,variable)];

%counts
for i = 1:size(combination,1),
    temp = repmat(combination(i,:),size(training_data,1),1);
    temp2 = sum(data==temp,2);
    counts(i) = sum(temp2==(length(parents)+1));
end;

% the sum of counts for the same combination of parents
counts_part_sum = zeros(size(counts,1)/3,size(counts,2));

%calculate counts_part_sum
for i = 1:length(counts_part_sum),
    counts_part_sum(i) = counts(3*(i-1)+1) + counts(3*(i-1)+2) + counts(3*(i-1)+3);
end;

score1 = sum(lgamma(counts + alpha/(3*3^length(parents))));
score2 = sum(lgamma(counts_part_sum + alpha/(3^length(parents))));
score = score1 - score2 + 3^length(parents)*lgamma(alpha/(3^length(parents))) - 3*3^length(parents)*lgamma(alpha/(3*3^length(parents)));
%}
score = randi(100);
