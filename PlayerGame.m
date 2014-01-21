classdef PlayerGame < handle
    properties
        STATE_SIZE = 4;
        STATE_DIM = 2;
        STATE_NUM = 16;
        STATE_SHAPE = [4,4];

        MAX_MESSAGE_LENGTH = 1024;
        DELIMITER = ' ';

        wall = [];
        owall = [];
        comm = '';
    end
    methods
        function msg = recv(obj, bytes)
            while obj.comm.BytesAvailable == 0 end
            if nargin < 2
                bytes = obj.comm.BytesAvailable;
            end
            data = fread(obj.comm, bytes, 'char');
            if ~data
                fprintf('END\n');
                exit;
            end
            msg = sprintf('%s', data);
        end
        function [] = send(obj, msg)
            fwrite(obj.comm, msg);
        end

        function obj = PlayerGame(GAMEID)
            %obj.comm = tcpip('localhost', 8080);
            obj.comm = tcpip('crisco.seas.harvard.edu', 8080);
            fopen(obj.comm);

            if strcmp(GAMEID, '0')
                GAMEID = randi([1000,9999])

            obj.send(GAMEID);
            fprintf(['Waiting for game ' obj.recv(length(GAMEID)) '\n']);
            fprintf([obj.recv(5) '\n']);

            obj.wall = reshape(str2num(obj.recv()), obj.STATE_SHAPE).';
            obj.owall = [];
        end
        function delete(obj)
            fclose(obj.comm);
        end

        function discard_brick = get_discard(obj)
            msg = obj.recv();
            if strcmp(msg, 'LOSE')
                fprintf(['***' msg '***\n']);
                exit;
            end
           data = str2num(msg);
           obj.owall = reshape(data(2:end), obj.STATE_SHAPE).';
           discard_brick = data(1);
        end
        function brick = get_brick(obj, move)
           obj.send(move);
           brick = str2num(obj.recv());
        end
        function [] = make_move(obj, move)
            obj.send(move);
            msg = obj.recv();
            if strcmp(msg, 'WIN')
                fprintf(['***' msg '***\n']);
                exit;
            end
            obj.wall = reshape(str2num(msg), obj.STATE_SHAPE).';
        end
    end   % END OF METHODS
end
