import connection
import ximu3

connection_info = ximu3.FileConnectionInfo("C:/x-IMU3 Example File.ximu3")  # replace with actual connection info

connection.run(connection_info)
