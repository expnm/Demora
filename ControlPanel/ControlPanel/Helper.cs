using System.Net;
using System.Net.Sockets;
using System.Text;

namespace ControlPanel
{
    internal class Helper
    {
        static Socket udpSocket;

        public static void SendMessage(string message, string ip, int port)
        {
            if (udpSocket == null)
            {
                udpSocket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            }

            byte[] data = Encoding.ASCII.GetBytes(message);
            udpSocket.SendTo(data, new IPEndPoint(IPAddress.Parse(ip), port));
        }
    }
}
