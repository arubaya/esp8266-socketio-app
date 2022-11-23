import { Server } from 'socket.io';

export default function SocketHandler(req: any, res: any) {
  if (res.socket.server.io) {
    console.log('Socket is already running');
    res.end();
  } else {
    console.log('Socket is initializing');
    const io = new Server(res.socket.server, {
      allowEIO3: true,
      cors: {
        origin: '*',
      },
    });
    res.socket.server.io = io;

    io.on('connection', (socket) => {
      console.log(
        `new user: ${socket.id} Origin: ${socket.handshake.headers.origin}`
      );
      socket.on('new-user', (message) => {
        console.log(message);
        socket.broadcast.emit(
          'broadcast-new-user',
          `user:${socket.id}, pesannya:${message}`
        );
      });

      socket.on('turn-led', (data) => {
        console.log(`LED status: ${data.status === 1 ? 'ON' : 'OFF'}`);
        socket.broadcast.emit('led-status', data);
        socket.broadcast.emit(
          'led-status-message',
          `LED status: ${data.status === 1 ? 'ON' : 'OFF'}`
        );
      });
    });
    res.end();
  }
}
