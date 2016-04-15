classdef ICAP_interface < handle
    %ICAP Summary of this class goes here
    %   Detailed explanation goes here
    
    properties (Hidden = true, SetAccess = private)
        cpp_handle;
    end
    
    methods
        % Constructor
        function this = ICAP_interface()
            this.cpp_handle = ICAP_mex('new');
            status = ICAP_mex('enable_rt_mode', this.cpp_handle);
            if (status ~= 0)
                this.delete();
            end
        end
        % Destructor
        function delete(this)
            if (this.cpp_handle < 0)
                error('Invalid ICAP handle')
            end
            r = ICAP_mex('delete', this.cpp_handle);
            this.cpp_handle = -1;
        end
        % This call will load the HPGs so it might take a bit to complete
        function status = open(this, inputFile, reportFile, statusFile)
            if (this.cpp_handle < 0)
                error('Invalid ICAP handle')
            end
            status = ICAP_mex('open', this.cpp_handle, inputFile, reportFile, statusFile);
            if (status ~= 0)
                this.delete();
                error('Error opening ICAP scenario');
            end
        end
        function status = start(this)
            if (this.cpp_handle < 0)
                error('Invalid ICAP handle')
            end
            status = ICAP_mex('start', this.cpp_handle);
            if (status ~= 0)
                this.delete();
                error('Error starting ICAP engine');
            end
        end
        function status = end(this)
            if (this.cpp_handle < 0)
                error('Invalid ICAP handle')
            end
            status = ICAP_mex('end', this.cpp_handle);
            if (status ~= 0)
                this.delete();
                error('Error ending ICAP simulation');
            end
        end
        function status = close(this)
            if (this.cpp_handle < 0)
                error('Invalid ICAP handle')
            end
            status = ICAP_mex('close', this.cpp_handle);
            if (status ~= 0)
                this.delete();
                error('Error closing ICAP engine');
            end
        end
        function status = clearNodeFlows(this)
            if (this.cpp_handle < 0)
                error('Invalid ICAP handle')
            end
            status = ICAP_mex('clear_node_flows', this.cpp_handle);
            if (status ~= 0)
                this.delete();
                error('Error clearing node flows');
            end
        end
        function status = setNodeFlow(this, nodeId, flow)
            if (this.cpp_handle < 0)
                error('Invalid ICAP handle')
            end
            status = ICAP_mex('set_node_flow', this.cpp_handle, nodeId, flow);
            if (status ~= 0)
                this.delete();
                error('Error setting the node flow');
            end
        end
        function [status, flow] = getNodeUpstreamInflows(this, nodeId)
            if (this.cpp_handle < 0)
                error('Invalid ICAP handle')
            end
            [status, flow] = ICAP_mex('get_node_us_inflows', this.cpp_handle, nodeId);
            if (status ~= 0)
                this.delete();
                error('Error getting the node upstream inflows');
            end
        end
        function [status, head] = getNodeHead(this, nodeId)
            if (this.cpp_handle < 0)
                error('Invalid ICAP handle')
            end
            [status, head] = ICAP_mex('get_node_head', this.cpp_handle, nodeId);
            if (status ~= 0)
                this.delete();
                error('Error getting the node head');
            end
        end
        function status = setNodeHead(this, nodeId, head)
            if (this.cpp_handle < 0)
                error('Invalid ICAP handle')
            end
            status = ICAP_mex('set_node_head', this.cpp_handle, nodeId, head);
            if (status ~= 0)
                this.delete();
                error('Error setting the node head');
            end
        end
        function status = addSource(this, nodeId)
            if (this.cpp_handle < 0)
                error('Invalid ICAP handle')
            end
            status = ICAP_mex('add_source', this.cpp_handle, nodeId);
            if (status ~= 0)
                this.delete();
                error('Error adding source');
            end
        end
        function [status, nextTimeStep] = step(this, dt)
            if (this.cpp_handle < 0)
                error('Invalid ICAP handle')
            end
            [status, nextTimeStep] = ICAP_mex('step', this.cpp_handle, dt);
            if (status ~= 0)
                this.delete();
                error('Error stepping code');
            end
        end
    end
    
end

