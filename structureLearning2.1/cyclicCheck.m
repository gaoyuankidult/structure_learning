%use topology sorting to check graph cyclic
function cyclic = cyclicCheck(connection)

%number of eliminated nodes

node_num = 0;
last_node_num = 0;

while 1,
    node_num = 0;
    
    for i = 1:length(connection),
        if sum(connection(:,i))==0,
            connection(i,:) = zeros(1,length(connection));
            node_num = node_num +1;
        endif;
    end;
    if last_node_num == node_num,
        break;
    endif;
    last_node_num = node_num;
endwhile;

if node_num==length(connection),
    cyclic = 0;
else
    cyclic = 1;
endif;
