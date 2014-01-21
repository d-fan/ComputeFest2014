import java.io.*;
import java.net.*;
import java.util.*;
import java.lang.*;

class Game
{
  private Socket comm;
  private DataInputStream in;
  private DataOutputStream out;

  private int MAX_MESSAGE_LENGTH = 1024;
  private byte[] buffer = new byte[MAX_MESSAGE_LENGTH];

  public int STATE_SIZE = 4;
  public int STATE_DIM = 2;
  public int STATE_NUM = (int) Math.pow(STATE_SIZE, STATE_DIM);

  public int[][] wall;
  public int[][] owall;

  private static String DELIMITER = new String(" ");

  private void send(String s) {
    try {
      out.write(s.getBytes(), 0, s.length());
    } catch(Exception e) {
      System.out.println("WRITE ERROR");
    }
  }

  private String recv() {
    int read_char = 0;
    try {
      read_char = in.read(buffer, 0, MAX_MESSAGE_LENGTH);
    } catch(Exception e) {
      System.out.println("READ ERROR");
    }
    String s = new String(buffer, 0, read_char);
    if (s.equals("")) {
      System.out.println("END");
      System.exit(0);
    }
    return s;
  }


  public String array2string(int[] array) {
    String temp = Arrays.toString(array).replace(", ", DELIMITER);
    return temp.substring(1, temp.length()-1);
  }

  public int[][] array2wall(int[] array, int offset) {
    int[][] wall = new int[STATE_SIZE][STATE_SIZE];
    for (int i = 0; i < STATE_SIZE; ++i)
      for (int j = 0; j < STATE_SIZE; ++j)
        wall[i][j] = array[offset++];
    return wall;
  }

  public int[] string2array(String string) {
    String[] items = string.split(DELIMITER);
    int[] results = new int[items.length];
    for (int i = 0; i < items.length; i++) {
      results[i] = Integer.parseInt(items[i]);
    }
    return results;
  }

  Game(String game_id) {
    try {
      comm = new Socket("crisco.seas.harvard.edu", 8080);
      //comm = new Socket("localhost", 8080);
      out = new DataOutputStream(comm.getOutputStream());
      in = new DataInputStream(comm.getInputStream());
    } catch(Exception e) {
      System.out.println("ERROR");
    }

    send(game_id);
    System.out.println("Waiting for game " + recv());

    String message = recv();
    System.out.println(message);
    assert message.equals("READY");

    // Receive our game wall
    wall = array2wall(string2array(recv()), 0);
  }

  public int get_discard() {
    String msg = recv();
    if (msg.equals("LOSE")) {
      System.out.println("***" + msg + "***");
      System.exit(0);
    }
    int[] data = string2array(msg);
    owall = array2wall(data, 1);
    return data[0];
  }

  public int get_brick(String move) {
    send(move);
    return Integer.parseInt(recv());
  }

  public void make_move(String move) {
    send(move);
    String msg = recv();
    if (msg.equals("WIN") ) {
      System.out.println("***" + msg + "***");
      System.exit(0);
    }
    wall = array2wall(string2array(msg), 0);
  }

}
