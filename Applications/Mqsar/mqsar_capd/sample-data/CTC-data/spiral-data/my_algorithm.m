function [test_targets, Wh, Wo, J] = my_algorithm(train_patterns, train_targets, test_patterns, params)

% Classify using a backpropagation network with a batch learning algorithm
% Inputs:
% 	training_patterns   - Train patterns
%	training_targets	- Train targets
%   test_patterns       - Test  patterns
%	params              - Number of hidden units, Convergence criterion, Convergence rate
%
% Outputs
%	test_targets        - Predicted targets
%   Wh                  - Hidden unit weights
%   Wo                  - Output unit weights
%   J                   - Error throughout the training


% train_patterns = train_patterns';
% x2_y2 = train_patterns(:, 1).^2 + train_patterns(:, 2).^2;
% %train_patterns = [train_patterns, x2_y2, 1./x2_y2]; 
% %train_patterns = [x2_y2, asin(train_patterns(:, 1)./sqrt(x2_y2))]; 
% train_targets = train_targets' + 1;
% test_patterns = test_patterns';
% x2_y2 = test_patterns(:, 1).^2 + test_patterns(:, 2).^2;
% %test_patterns = [test_patterns, x2_y2, 1./x2_y2];
% %test_patterns = [x2_y2, asin(test_patterns(:, 1)./sqrt(x2_y2))];
% input_min = min(train_patterns)';
% input_max = max(train_patterns)';
% tic;
% net = newff([input_min input_max], [size(train_patterns, 2) 6 4 2], ...
%     {'purelin', 'tansig', 'tansig', 'purelin'}, 'trainlm');
% %net = newcf([input_min input_max], [size(train_patterns, 2) 1 1 1 1 1 1 1 1 1 1 1 1 1]);
% 
% net.trainParam.epochs = 10000;
% net.trainParam.show = nan;
% 
% num_class = max(train_targets);
% train_targets = repmat(train_targets, 1, num_class);
% for idx = 1:num_class
%     train_targets(train_targets(:, idx) ~= idx, idx) = -1;
%     train_targets(train_targets(:, idx) == idx, idx) = 1;
% end
% 
% net = train(net, train_patterns', train_targets');
% net_bin_tr = sim(net, test_patterns')';
%  [nouse, idx]= max(net_bin_tr, [], 2); % maximum for each data
%  net_bin_tr = idx;
% 
% test_targets = net_bin_tr' - 1;
% 
% Wh = [];
% Wo = [];
% J = [];
% toc;
% return;


Wh = [];
Wo = [];
J = [];

Uc      = length(unique(train_targets));

%If there are only two classes, remap to {-1,1}
if (Uc == 2)
    train_targets    = (train_targets>0)*2-1;
end

patterns = train_patterns';
targets = train_targets';

W = init_crnet(patterns, targets, params);
accuracy = 0;
iter = 0;
E = 0;
inc_E = 1.0;
threshold = 0.001;

tic;
while(inc_E > threshold || inc_E < 0 || iter < 15)
    iter = iter + 1;
    Wi_candidate = train_candidate(W, patterns, targets, params);

    % plug in the candidate
    if size(W, 2) == 1
        W = [Wi_candidate, W; 0, 0];
        W(end, end) = W(end - 1, end);
        W(end - 1, end) = rand();  % weight from candidate to output node.
    else
        W = [W(:, 1:end - 1), Wi_candidate, W(:, end)];
        pad_zero_row = zeros(1, size(W, 2));
        W = [W; pad_zero_row];
        W(end, end) = W(end - 1, end);
        W(end - 1, end) = rand();  % weight from candidate to output node.
    end

    % train the weight from candidate to output node
    Wo = train_Wo(W, patterns, targets, params);
    W(:, end) = Wo;

    % error
    [output, nouse] = get_output(patterns, W);
    old_E = E;
    E = sum((output - targets).^2)/size(output, 1);
    inc_E = old_E - E;
    disp(sprintf('%g: mean square error: %g', iter, E));

    output = get_output(patterns, W);
    output(output > 0) = 1;
    output(output < 0) = -1;
    accuracy = sum(output == train_targets')/size(output, 1);

    disp(sprintf('%g: accuracy: %g', iter, accuracy));

end

toc;

test_targets = get_output(test_patterns', W);
test_targets(test_targets > 0) = 1;
test_targets(test_targets < 0) = 0;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
function Wo = init_crnet(train_patterns, train_targets, params)

train_patterns = train_patterns';
train_targets = train_targets';

[Nh, Theta, eta, mu] = process_params(params);
iter	= 1;
DispIter = 10;

[Ni, M] = size(train_patterns);
No      = 1;

%Initialize the net: In this implementation there is only one output unit, so there
%will be a weight vector from the hidden units to the output units, and a weight matrix
%from the input units to the hidden units.
%The matrices are defined with one more weight so that there will be a bias
w0			= max(abs(std(train_patterns)'));
Wo			= rand(No,  Ni).*w0*2-w0; %Output weights
Wo          = [Wo, 1];
Wo          = Wo/mean(std(Wo'))*(Ni+1)^(-0.5);

OldDeltaWo  = zeros(size(Wo));
deltaJo     = zeros(size(Wo));
OldDeltaJo  = zeros(size(Wo));

J(1)       	= 1e3;
rate        = Theta*10;

while (rate > Theta),
    OldDeltaJo  = deltaJo;
    deltaJo     = zeros(size(Wo));
   
    for m = 1:M,
        Xm = train_patterns(:, m);
        tk = train_targets(m);
        
        %Forward propagate the input:
        %First to the hidden units
        gh				= Wo*[Xm; 1];
        [zk, dfo]		= activation(gh);
        
        %Now, evaluate delta_k at the output: delta_k = (tk-zk)*f'(net)
        delta_k		= (tk - zk).*dfo;
        
        %delta_w_kj <- w_kj + eta*delta_k*y_j
        deltaJo		= deltaJo + delta_k*[Xm;1]';
    end
    
    %delta_w <- delta_J(m)/(delta_J(m-1)-delta_J(m))*delta_w(m)
    %Well, it's not that simple. For details see "Back Propagation Family Album" by Jondarr Gibb. 
    %Dept. of Computing, Macquarie University, Technical report C/TR95-05, 1996.
    deltaWo     = zeros(size(Wo));
    for i = 1:size(Wo,1),
        for j = 1:size(Wo,2),
            if (OldDeltaWo(i,j) > 0),
                if (deltaJo(i,j) > 0),
                    deltaWo(i,j) = eta * deltaJo(i,j);
                end
                if (deltaJo(i,j) > mu/(mu+1)*OldDeltaJo(i,j)),
                    deltaWo(i,j) = deltaWo(i,j) + mu*OldDeltaWo(i,j);
                else
                    deltaWo(i,j) = deltaWo(i,j) + deltaJo(i,j) * OldDeltaWo(i,j) / (OldDeltaJo(i,j) - deltaJo(i,j));
                end
            else
                if (OldDeltaWo(i,j) < 0),
                    if (deltaJo(i,j) < 0),
                        deltaWo(i,j) = eta * deltaJo(i,j);
                    end
                    if (deltaJo(i,j) < mu/(mu+1)*OldDeltaJo(i,j)),
                        deltaWo(i,j) = deltaWo(i,j) + mu*OldDeltaWo(i,j);
                    else
                        deltaWo(i,j) = deltaWo(i,j) + deltaJo(i,j) * OldDeltaWo(i,j) / (OldDeltaJo(i,j) - deltaJo(i,j));
                    end
                else
                    deltaWo(i,j) = eta * deltaJo(i,j);
                end
            end
        end
    end

    Wo = Wo + deltaWo;
    OldDeltaWo = deltaWo;

    iter 			= iter + 1;

    %Calculate total error
    J(iter)    = 0;
    for i = 1:M,
        J(iter) = J(iter) + ((train_targets(i)' - activation(Wo*[train_patterns(:,i); 1])).^2);
    end
    J(iter)    = J(iter)/M;
    rate = abs(J(iter) - J(iter-1))/J(iter-1)*100;

%     if (iter/DispIter == floor(iter/DispIter)),
%         disp(['init_crnet: Iteration ' num2str(iter) ': Total error is ' num2str(J(iter))]);
%     end
    
end

Wo = Wo';

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
function input_W = train_candidate(W, patterns, targets, params)

[Nh, Theta, eta, mu] = process_params(params);
iter	= 1;

% init input weight for candidate
No = 1;
Ni = size(W, 1);
w0			= max(abs(std(patterns)'));
Wi			= rand(Ni, No).*w0*2-w0; %Output weights
%Wi = [Wi; 1];
Wi(end) = 1;
Wi          = Wi/mean(std(Wi))*(Ni+1)^(-0.5);

% W(:, end) = Wi;
% [output, last_input] = get_output(patterns, W);
% E = (output - targets).^2;
% mean_E = sum(E)/size(E, 1);
% mean_V = sum(output)/size(output, 1);
% S(1) = abs(sum((output - mean_V).*(E - mean_E)));
% disp(sprintf('train candidate: coorelation for iterative %g is: %g', iter, S(iter)));

S(1) = 1;
rate = Theta + 1;

% train input weight
while (rate > Theta)
    iter = iter + 1;
    %W(:, end) = Wi;
    [output, last_input] = get_output(patterns, W);
    E = (output - targets);
    mean_E = sum(E)/size(E, 1);

    [V, f_p_prime] = activation(last_input * Wi);
    S(iter) = sum((V - mean(V)).*(E - mean_E));
    sigma = 2 * (S(iter) > 0) - 1;  % S > 0 --> 1; S < 0 --> -1;

    % I_i_p = last_input;
    dS_over_dWi = sigma * ((E - mean(E)).*f_p_prime)' * last_input;
    dS_over_dWi = dS_over_dWi';
    input_W = Wi;
    Wi = Wi + eta * dS_over_dWi;

    S(iter) = abs(S(iter));

    DispIter         = 10;
%     if (iter/DispIter == floor(iter/DispIter)),
%         disp(sprintf('candidate weight: coorelation for iteration %g is: %g', iter, S(iter)));
%     end

    rate = (S(iter) - S(iter-1))/S(iter-1)*100;
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
function Wo = train_Wo(W, train_patterns, train_targets, params)

train_patterns = train_patterns';
train_targets = train_targets';

[Nh, Theta, eta, mu] = process_params(params);
iter	= 1;
DispIter         = 10;

[Ni, M] = size(train_patterns);
No      = 1;

%Initialize the net: In this implementation there is only one output unit, so there
%will be a weight vector from the hidden units to the output units, and a weight matrix
%from the input units to the hidden units.
%The matrices are defined with one more weight so that there will be a bias
% w0			= max(abs(std(train_patterns)'));
% t			= rand(No,  size(W, 1)).*w0*2-w0; %Output weights
% t          = t/mean(std(t'))*(Ni+1)^(-0.5);
% W(:, end) = t;
Wo = W(:, end)';

OldDeltaWo  = zeros(size(Wo));
deltaJo     = zeros(size(Wo));
OldDeltaJo  = zeros(size(Wo));

J(1)       	= 1e3;
rate        = Theta*10;

min_error = 1e3;  % arbitrary large number
min_error_Wo = [];
run_count = 0;

while (~(rate < Theta))
    run_count = run_count + 1;
    OldDeltaJo  = deltaJo;
    deltaJo     = zeros(size(Wo));
   
    for m = 1:M,
        Xm = train_patterns(:, m);
        [nouse, Xm] = get_output(Xm', W);
        Xm = Xm';
        tk = train_targets(m);
        
        %Forward propagate the input:
        %First to the hidden units
        gh				= Wo*Xm;
        [zk, dfo]		= activation(gh);
        
        %Now, evaluate delta_k at the output: delta_k = (tk-zk)*f'(net)
        delta_k		= (tk - zk).*dfo;
        
        %delta_w_kj <- w_kj + eta*delta_k*y_j
        deltaJo		= deltaJo + delta_k*Xm';
    end
    
    %delta_w <- delta_J(m)/(delta_J(m-1)-delta_J(m))*delta_w(m)
    %Well, it's not that simple. For details see "Back Propagation Family Album" by Jondarr Gibb. 
    %Dept. of Computing, Macquarie University, Technical report C/TR95-05, 1996.
    deltaWo     = zeros(size(Wo));
    for i = 1:size(Wo,1),
        for j = 1:size(Wo,2),
            if (OldDeltaWo(i,j) > 0),
                if (deltaJo(i,j) > 0),
                    deltaWo(i,j) = eta * deltaJo(i,j);
                end
                if (deltaJo(i,j) > mu/(mu+1)*OldDeltaJo(i,j)),
                    deltaWo(i,j) = deltaWo(i,j) + mu*OldDeltaWo(i,j);
                else
                    deltaWo(i,j) = deltaWo(i,j) + deltaJo(i,j) * OldDeltaWo(i,j) / (OldDeltaJo(i,j) - deltaJo(i,j));
                end
            else
                if (OldDeltaWo(i,j) < 0),
                    if (deltaJo(i,j) < 0),
                        deltaWo(i,j) = eta * deltaJo(i,j);
                    end
                    if (deltaJo(i,j) < mu/(mu+1)*OldDeltaJo(i,j)),
                        deltaWo(i,j) = deltaWo(i,j) + mu*OldDeltaWo(i,j);
                    else
                        deltaWo(i,j) = deltaWo(i,j) + deltaJo(i,j) * OldDeltaWo(i,j) / (OldDeltaJo(i,j) - deltaJo(i,j));
                    end
                else
                    deltaWo(i,j) = eta * deltaJo(i,j);
                end
            end
        end
    end

    Wo = Wo + deltaWo;
    W(:, end) = Wo';
    OldDeltaWo = deltaWo;

    iter 			= iter + 1;

    %Calculate total error
    [output, last_input] = get_output(train_patterns', W);
    J(iter) = sum((train_targets' - output).^2);
%     J(iter)    = 0;
%     for i = 1:M,
%         J(iter) = J(iter) + ((train_targets(i)' - activation(Wo*last_input(i, :)')).^2);
%     end
    
    J(iter)    = J(iter)/M;
    if J(iter) < min_error
        min_error = J(iter);
        min_error_Wo = Wo;
    end
    rate = abs(J(iter) - J(iter-1))/J(iter-1)*100;

%     DispIter         = 10;
%     if (iter/DispIter == floor(iter/DispIter)),
%         disp(['output weight: Iteration ' num2str(iter) ': Total error is ' num2str(J(iter))])
%     end
end

Wo = min_error_Wo';

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
function [output, last_input] = get_output(patterns, W)

start_insert_idx = size(patterns, 2);
pad_ones = ones(size(patterns, 1), size(W, 1) - size(patterns, 2));
patterns = [patterns, pad_ones];
output = activation(patterns*W(:, 1));
for idx = 2:size(W, 2)
    start_insert_idx = start_insert_idx + 1;
    patterns(:, start_insert_idx) = output;
    output = activation(patterns * W(:, idx));
end
last_input = patterns;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%
function [f, df] = activation(x)

a = 1.716;
b = 2/3;
f	= a*tanh(b*x);
df	= a*b*sech(b*x).^2;



