function probability(variable,connection,test_data)

%find parents
[x,y] = find(connection == 1);
xy = [x,y];
parents = xy(find(xy(:,2)==variable),1);


