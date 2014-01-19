using System;
using System.IO;

namespace TheResistorNetwork.ETD
{
    public static class MapTools 
    {
        public static void Main(string[] args)
        {
			/*
            Console.WriteLine("Map Tools for ETD");
            Console.WriteLine("Converts a map file to a header file.");
            
            if(args.Length < 1)
            {
                Console.WriteLine("Usage: maptools.exe mapfile");
                return;
            }
             */

			Console.WriteLine("const uint8_t mapTiles[] PROGMEM = {");

            var filename = args[0];
			using(var file = File.OpenText(filename))
            {
                string line;
				var tileNum = 0;
				var tileVal = 0;
                
                while((line = file.ReadLine()) != null)
                {
					foreach (var c in line.ToCharArray())
					{
						byte tile = 0;

						switch (c)
						{
							case '#':
								tile = 0x01;
								break;
							case '~':
								tile = 0x02;
								break;
							case '.':
								tile = 0x03;
								break;
							default:
								tile = 0x00;
								break;
						}

						tileVal |= (tile << (tileNum * 2));

						if (++tileNum > 3) {
							tileNum = 0;
							Console.WriteLine ("0x{0},", tileVal.ToString ("X2"));
							tileVal = 0;
						}
					}
                }

				Console.WriteLine("};");
            }
        }
    }
}
