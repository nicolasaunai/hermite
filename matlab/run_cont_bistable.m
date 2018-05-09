 function run_cont_bistable
    
    global N m SS
          
    lam0= 10;                                                               % initial guess for beta
    dlam=0.05;                                                              % maximum step on arclength continuation
    theta = 0.5;   
            
    % space discretisaiton (more details in steady_cont_bistable.m)
    L = 10;                                                                 % x in (-L/2,L/2)
    dx = 0.05;
    N = L/dx;
    x = dx*(0:N);
    x = x-L/2;
    x = x';        

    
    % initialise guess
    m = -1;                                                                % guess for first moment
    V = @(y) y.^4/4 - y.^2/2;                                               % bistable potential
    SS = exp(-lam0*(V(x)+theta*x.^2/2 - theta*x*m));                   % guess for solution with first moment xi
    SS = SS/trapz(x,SS);                                                    % rescaling
    
    u_in = [SS ; m];                               
    
    MON = [];               % to store the first moment (solution to R(m)=m
    LAM = [];               % stores values of beta
    P   = [];               % stores the solution
    tic
    [SOL,Mon,S] = cont(@steady_cont_bistable,u_in,lam0,60,...
        'MonitorFcn',@Monitor,'step',dlam,'direction',1,...         
        'jacobian','off','minstep',1e-6,'tolfun',1e-8,...
        'algorithm','keller',...
        'stop','on');
        %% 'Adaptative',{[N 5 1e-8]},'Maxstep',1,...
    toc         
    %% the following plots the bifurcation diagram 
    function g = Monitor(u,lambda)
       g = u(end);
       ss = u(1:end-1);
       p = exp(-beta*(V(x)+theta*x.^2/2-theta*x*g));
       p = p/trapz(x,p);
       LAM = [LAM,lambda];
       MON = [MON,g];
       P = [P,ss];
       subplot(2,1,1)
       plot(x,ss,'b-',x,p,'r--','linewidth',2)
       xlabel('$x$', 'interpreter','latex'); ylabel('$p_{\infty}(x)$', 'interpreter','latex')
       subplot(2,1,2)
       plot(LAM,MON,'linewidth',2)
       xlabel('$\beta$', 'interpreter','latex'); ylabel('$m$', 'interpreter','latex')
       drawnow
    end
% save('bifurcation_diagram.mat','MON','LAM','P');

 end