
function RunICAP(inpFile, rptFile, outFile)

%% Initialization Stuff
h = ICAP_mex('new');
r = ICAP_mex('enable_rt_mode', h);
if (r ~= 0)
    r = ICAP_mex('delete', h);
    return;
end
r = ICAP_mex('open', h, inpFile, rptFile, outFile);
if (r ~= 0)
    r = ICAP_mex('delete', h);
    return;
end
r = ICAP_mex('start', h);
if (r ~= 0)
    r = ICAP_mex('delete', h);
    return;
end

%% Execution Stuff
node = '3896';
ts = [0 1000 2000 4000 2000 1000 0];
t = 0.0000001;
idx = 1;
while (t > 1e-20)
    if (idx <= length(ts))
        r = ICAP_mex('set_node_flow', h, node, ts(idx));
        if (r ~= 0)
            r = ICAP_mex('delete', h);
            return;
        end
    end
    [r, t] = ICAP_mex('step', h);
    if (r ~= 0)
        r = ICAP_mex('delete', h);
        return;
    end
    
    fprintf('Now at %f minutes\n', t * 1440);
    idx = idx + 1;
end

r = ICAP_mex('end', h);
if (r ~= 0)
    r = ICAP_mex('delete', h);
    return;
end

r = ICAP_mex('close', h);
if (r ~= 0)
    r = ICAP_mex('delete', h);
    return;
end

r = ICAP_mex('delete', h);


end
