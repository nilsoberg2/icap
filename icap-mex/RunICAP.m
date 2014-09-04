function RunICAP

inpFile = 'lawrence_july.inp';
rptFile = 'lawrence_july.rpt';
outFile = 'lawrence_july.out';

% Create an ICAP interface
icap = ICAP_interface();

% Open the ICAP input file and output files
icap.open(inpFile, rptFile, outFile);

% Start the simulation engine (e.g. turn on the car)
icap.start();

% Now add a list of all of the input nodes (e.g. dropshafts)
node = 'Melvina';
icap.addSource(node);

% Fake timeseries
ts = [0 1000 2000 4000 2000 1000 0];
idx = 1;

% The ICAP step function returns zero time when the simulation has concluded. 
% The simulation end date can be set as long in the future as desired.
t = 0.0000001;
while (idx < 9)
    
    outNode = 'Outlet';
	
	% Set the head at the downstream node
    r = icap.setNodeHead(outNode, 380);
    
	% Clear the existing flows and assign new node flows at specific nodes
    icap.clearNodeFlows();
    r = icap.setNodeFlow('Melvina', idx * 100);
    
	% Execute a timestep with a DT of 1 second
    [r, icapT] = icap.step(1);
	
	% Get the head at the downstream-most node (can be any)
    [r, head] = icap.getNodeHead(outNode);
    fprintf('Head at %s: %f\n', outNode, head);
    
%    if (idx <= length(ts))% && head < -235)
%        icap.setNodeFlow('Melvina', ts(idx));
%    end
	
	[r, flow] = icap.getNodeUpstreamInflows(outNode);
    fprintf('US inflow at %s: %f\n', outNode, flow);
    
    fprintf('Now at %f minutes\n', t * 1440);
    idx = idx + 1;
    
end

% Wrap up the simulation
icap.end();
icap.close();

% Clear the object
clear icap;

end
