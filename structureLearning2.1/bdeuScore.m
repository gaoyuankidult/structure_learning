%a function for counting the counts of samples and calculating BDeu score for the input variables
function score = bdeuScore(variable, parents, alpha,training_data)
%score = randi(100);


combination = [variable; fliplr(parents)];

%initialize the counts of combinations of variables
counts = zeros(3 ^ (1 + length(parents)), 1);

for i = 1:size(training_data,1),
    %for a specific combination, it is stored in a specific row
    row_num = 1;
    for j = 1:length(combination),
        row_num = row_num + (training_data(i,combination(j)) - 1) * 3^(j-1);
    end;
    counts(row_num) = counts(row_num) + 1;
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


%{score = randi(100);%}
