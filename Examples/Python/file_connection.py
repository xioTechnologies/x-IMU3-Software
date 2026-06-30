import connection
import ximu3

config = ximu3.FileConnectionConfig("C:/Users/Public/x-IMU3 Example File.ximu3")  # replace with actual connection config

connection.run(config)
