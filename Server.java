import java.net.*;
import java.io.*;

public class Server {
    public static void main(String[] args) throws IOException {
        ServerSocket ss = new ServerSocket(5009);
        System.out.println("Server started, waiting for client...");

        while (true) {
            Socket s = ss.accept();
            System.out.println("Client connected ! " + s.getInetAddress());
            new ClientHandler(s).start();
        }
    }

    public static class ClientHandler extends Thread {
        Socket socket;
        ClientHandler(Socket s) {
            socket = s;
        }
        @Override
        public void run() {
            try (
                BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                PrintWriter out = new PrintWriter(socket.getOutputStream(), true)
            ) {
                String msg;
                System.out.println("Handler started for " + socket.getInetAddress());

                while ((msg = in.readLine()) != null) {
                    System.out.println("Received from " + socket.getInetAddress() + ": " + msg);
                    out.println("Echo: " + msg);
                    if (msg.equalsIgnoreCase("exit")) {
                        System.out.println("Client " + socket.getInetAddress() + " disconnected.");
                        break;
                    }
                }
            } catch (Exception e) {
                System.out.println("Error: " + e.getMessage());
            }
        }
    }
}
