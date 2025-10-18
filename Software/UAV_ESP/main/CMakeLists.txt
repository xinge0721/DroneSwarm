idf_component_register(SRCS "main.cpp" 
                            "../Hardware/WIFI/WIFI.cpp"
                            "../Hardware/UDP/UDP.cpp"
                            "../Hardware/MPU6050/MPU6050.cpp"
                            "../Hardware/TIME/TIME.cpp"
                            "../System/delay/delay.cpp"
                            "../System/sys/sys.cpp"
                       INCLUDE_DIRS "." "../Hardware" "../System")
