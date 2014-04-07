%a function for counting the counts of samples and calculating MDL score for the input variables
function score = mdlScore(variable, parents)

training_data = load('training_data2');

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

%a part of MDLScore
score1 = log(sum(counts)) * 3^length(parents);

% the sum of counts for the same combination of parents
counts_part_sum = zeros(size(counts));

for i = 1:3:length(counts),
    counts_part_sum(i) = counts(i) + counts(i+1) + counts(i+2);
    counts_part_sum(i+1) = counts(i) + counts(i+1) + counts(i+2);
    counts_part_sum(i+2) = counts(i) + counts(i+1) + counts(i+2);
    %----in case that count is zero------------------------------
    if counts_part_sum(i)==0,
        counts(i) = 1;
        counts(i+1) = 1;
        counts(i+2) = 1;
        counts_part_sum(i) = 1;
        counts_part_sum(i+1) = 1;
        counts_part_sum(i+2) = 1;
    endif;
    if counts(i) == 0,
        counts(i) = counts_part_sum(i);
    endif;
    if counts(i+1) == 0,
        counts(i+1) = counts_part_sum(i+1);
    endif;
    if counts(i+2) == 0,
        counts(i+2) = counts_part_sum(i+2);
    endif;
    %------------------------------------------------------------
end;

%the other part of MDLScore
score2 = sum(log(counts ./ counts_part_sum) .* counts);

score = score1 - score2;

