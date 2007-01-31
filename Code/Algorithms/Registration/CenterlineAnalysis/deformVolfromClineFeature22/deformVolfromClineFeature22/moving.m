clear all;
close all;

moving_data1 = load('matlab_testCV1.txt','-ascii');
[moving_num1, dummy] = size(moving_data1)
moving_data2 = load('matlab_testCV2.txt','-ascii');
[moving_num2, dummy] = size(moving_data2)


m1x = moving_data1(:,1);
m1y = moving_data1(:,2);
m1z = moving_data1(:,3);

m2x = moving_data2(:,1);
m2y = moving_data2(:,2);
m2z = moving_data2(:,3);

viewS = 113;%530;%571;%75;%494;%494;%324;%654;%494;%
viewL = 132;%610;%650;%82;%501;%367;%727;%501;%

h = zeros(moving_num1,1); h(viewS,1) = m1y(viewS,1);
k = zeros(moving_num2,1); k(viewL,1) = m2y(viewL,1);
hh = zeros(moving_num1,1); hh(viewS,1) = m1z(viewS,1);
kk = zeros(moving_num2,1); kk(viewL,1) = m2z(viewL,1);
figure(1); 
subplot(3,1,1); plot(m1x,'r'); hold on; plot(m2x,'b'); title('moving averaged x'); 
subplot(3,1,2); plot(m1y,'r'); hold on; plot(m2y,'b'); title('moving averaged y');
plot(h,'c');
plot(k,'m');
subplot(3,1,3); plot(m1z,'r'); hold on; plot(m2z,'b'); title('moving averaged z');
plot(hh,'c');
plot(kk,'m');
legend('source','target');


max_data1 = load('matlab_testMax1.txt','-ascii');
[max_num1, dummy] = size(max_data1);
max_data2 = load('matlab_testMax2.txt','-ascii');
[max_num2, dummy] = size(max_data2);

max_data1(find(abs(max_data1)>=10))=0;
max1x = max_data1(:,1);
max1y = max_data1(:,2);
max1z = max_data1(:,3);

max_data2(find(abs(max_data2)>=10))=0;
max2x = max_data2(:,1);
max2y = max_data2(:,2);
max2z = max_data2(:,3);
h4 = zeros(moving_num1,1); h4(viewS,1) = max1y(viewS,1);
k4 = zeros(moving_num2,1); k4(viewL,1) = max2y(viewL,1);
hhh = zeros(moving_num1,1); hhh(viewS,1) = max1z(viewS,1);
kkk = zeros(moving_num2,1); kkk(viewL,1) = max2z(viewL,1);
figure(2); 

subplot(3,1,1); plot(max1x,'r'); hold on; plot(max2x,'b'); title('maxima for the moving averaged x'); 
subplot(3,1,2); plot(max1y,'r'); hold on; plot(max2y,'b'); title('maxima for the moving averaged y');
plot(k4,'m'); plot(h4,'c');
subplot(3,1,3); plot(max1z,'r'); hold on; plot(max2z,'b'); title('maxima for the moving averaged z');
plot(kkk,'m');
plot(hhh,'c');
legend('source','target');