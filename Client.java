import java.io.*;
import java.net.*;
import java.util.*;

public class Client {
    public static void main(String[] args) throws IOException {
        try (Socket socket = new Socket("localhost", 5009)) {
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            Scanner sc = new Scanner(System.in);

            while (true) {
                System.out.print("Enter a msg: ");
                String msg = sc.nextLine();
                out.println(msg);

                if (msg.equalsIgnoreCase("exit")) {
                    System.out.println("Connection closed.");
                    break;
                }

                String response = in.readLine();
                System.out.println("Echo from server: " + response);
            }
        } catch (Exception e) {
            System.out.println("Error message: " + e.getMessage());
        }
    }
}
