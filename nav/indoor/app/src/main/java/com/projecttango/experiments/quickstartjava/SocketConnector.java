package com.projecttango.experiments.quickstartjava;
package com.example.androidclient;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.net.UnknownHostException;

import android.os.AsyncTask;
import android.os.Bundle;

public class SocketConnection {
  String dstAddress;
  int dstPort;
  Socket socket;

  SocketConnection(String addr, int port) {
    dstAddress = addr;
    dstPort = port;
    socket = new Socket(dstAddress, dstPort);
  }

  public void send(String msg) {
    try {
      PrintWriter out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(
        socket.getOutputStream())), true);
      out.println(msg);
    } catch (Exception e) {
    }
  }

  public void close() {
    socket.close();
  }
}
