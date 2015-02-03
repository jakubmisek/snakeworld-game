using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace SnakeWorld_Server
{
    /// <summary>
    /// Commands IDs
    /// </summary>
    public enum ECmdIds
    {
        CMD_NAME = 1,
        CMD_DESCRIPTION = 2,
        CMD_TYPE = 3,
        CMD_TEXTURE = 4,
        CMD_LANGUAGE = 5,
        CMD_SERVER = 6,
        CMD_YOURSNAKEID = 7,
        CMD_IDONTKNOW = 8,
        CMD_SCENE = 9,
        CMD_ERROR = 10,         // [error string] an error occurred and connection will be closed

        CMD_RECEIVING_SNAKE = 12,
        CMD_SNAKE_HEAD = 13,
        CMD_SNAKE_DIR = 14,
        CMD_SNAKE_SPEED = 15,
        CMD_SNAKE_LENGTH = 16,
        CMD_SNAKE_SEG = 17,
        CMD_SNAKE_DEAD = 18,
        CMD_SNAKE_BOUND = 19,
        CMD_SNAKE_IDONWKNOW = 20,

        CMD_NEWSNAKE = 21,

        CMD_BESTSNAKE = 22,

        CMD_APPLE = 23,         // [apple id,x,y] apple position, implicates that apple exists and defines his position
        CMD_APPLEEATEN = 24,    // [apple id] apple eaten by snake that sends this command

        CMD_CHATMESSAGE = 25,	// client send:[to user name,message]		|	client receive:[from user name,message]

        CMD_LOGIN = 26,   // [users email address][users password (hashed)]

        CMD_BLOCKEND = 255	// command block end
    };

    /// <summary>
    /// Commands sending/receiving helper functions
    /// </summary>
    public class ConnectionData
    {
        /// <summary>
        /// NEW GAME REQUEST
        /// </summary>
        internal const string CMD_NEWGAMECMD = "NEWGAME";

        /// <summary>
        /// GET TEXTURE REQUEST
        /// </summary>
        internal const string CMD_GETTEXTURE = "GETTEXTURE";

        /// <summary>
        /// Send the string into the binary writer
        /// </summary>
        /// <param name="bw">binary writer</param>
        /// <param name="str">string to be sent</param>
        internal static void Send_String(BinaryWriter bw, string str)
        {
            foreach (char c in str)
                bw.Write((UInt16)c);  // unicode 16-bits character

            bw.Write((UInt16)0);      // 16-bits zero
        }

        /// <summary>
        /// send the command header
        /// </summary>
        /// <param name="bw">binary writer</param>
        /// <param name="e">command to be sent</param>
        internal static void Send_CmdId(BinaryWriter bw, ECmdIds e)
        {
            bw.Write((byte)e);
        }

        /// <summary>
        /// receive the string from the stream
        /// </summary>
        /// <param name="br">reader</param>
        /// <returns>zero terminated string from the stream</returns>
        internal static string Receive_String(BinaryReader br)
        {
            string str = "";

            char c;

            while(true)
            {
                c = (char)br.ReadUInt16();

                if (c != 0)
                {
                    str += c;
                }
                else
                    break;
            }

            return str;
        }
    }
}
