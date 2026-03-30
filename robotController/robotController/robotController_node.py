#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32
from geometry_msgs.msg import Twist
import serial
import serial.tools.list_ports
import time
import math

class ArduinoController(Node):
    def __init__(self):
        super().__init__('arduino_controller')

        # Параметры
        self.declare_parameter('port', '')
        self.declare_parameter('baudrate', 9600)
        self.declare_parameter('rate', 10.0)
        self.declare_parameter('read_rate', 20.0)
        self.declare_parameter('enc_topic_left', 'encoder_left')
        self.declare_parameter('enc_topic_right', 'encoder_right')
        self.declare_parameter('reconnect_delay', 2.0)
        # Расстояние между колесами в метрах (необходимо для расчета кинематики)
        self.declare_parameter('wheel_base', 0.20) 

        self.port = self.get_parameter('port').get_parameter_value().string_value
        self.baudrate = self.get_parameter('baudrate').get_parameter_value().integer_value
        self.rate = self.get_parameter('rate').get_parameter_value().double_value
        self.read_rate = self.get_parameter('read_rate').get_parameter_value().double_value
        self.enc_topic_left = self.get_parameter('enc_topic_left').get_parameter_value().string_value
        self.enc_topic_right = self.get_parameter('enc_topic_right').get_parameter_value().string_value
        self.reconnect_delay = self.get_parameter('reconnect_delay').get_parameter_value().double_value
        self.wheel_base = self.get_parameter('wheel_base').get_parameter_value().double_value

        # Переменные для хранения скоростей моторов (в см/с)
        self.speed_left = 0
        self.speed_right = 0

        # Подписка на стандартный топик навигации
        self.sub_cmd_vel = self.create_subscription(Twist, '/cmd_vel', self.cmd_vel_callback, 10)

        # Публикаторы для энкодеров
        self.pub_enc_left = self.create_publisher(Int32, self.enc_topic_left, 10)
        self.pub_enc_right = self.create_publisher(Int32, self.enc_topic_right, 10)

        # Состояние подключения
        self.serial_conn = None
        self.last_reconnect_attempt = 0.0
        self.connect_serial()

        # Таймеры
        self.timer_send = self.create_timer(1.0 / self.rate, self.send_commands)
        self.timer_read = self.create_timer(1.0 / self.read_rate, self.read_serial)

        self.get_logger().info('Arduino controller node started (ROS 2 Jazzy)')
        self.get_logger().info(f'Wheel base set to: {self.wheel_base} meters')

    def connect_serial(self):
        """Устанавливает соединение с Arduino."""
        if self.serial_conn is not None:
            try:
                self.serial_conn.close()
            except:
                pass
            self.serial_conn = None

        port = self.port
        if not port:
            # Автоматическое определение
            ports = serial.tools.list_ports.comports()
            self.get_logger().info(f'Found ports: {[p.device for p in ports]}')
            arduino_ports = [p.device for p in ports if 'Arduino' in p.description or 'usb' in p.device]
            if not arduino_ports:
                self.get_logger().error('Arduino not found')
                return
            port = arduino_ports[0]
            self.get_logger().info(f'Using automatically detected port: {port}')

        try:
            self.serial_conn = serial.Serial(port, self.baudrate, timeout=0.1)
            time.sleep(2)  # даём Arduino время на инициализацию
            self.get_logger().info(f'Connected to {port} at {self.baudrate} baud')
            self.last_reconnect_attempt = self.get_clock().now().nanoseconds / 1e9
        except Exception as e:
            self.get_logger().error(f'Failed to open serial port: {e}')
            self.serial_conn = None

    def reconnect_serial(self):
        """Пытается переподключиться, но не чаще чем раз в reconnect_delay секунд."""
        now = self.get_clock().now().nanoseconds / 1e9
        if now - self.last_reconnect_attempt >= self.reconnect_delay:
            self.get_logger().warn('Attempting to reconnect to Arduino...')
            self.connect_serial()
            self.last_reconnect_attempt = now
        else:
            self.get_logger().debug('Reconnect attempt too soon, skipping')

    def cmd_vel_callback(self, msg):
        """
        Обрабатывает команды движения.
        Конвертирует м/с и рад/с в см/с и вычисляет скорости для левого/правого колеса.
        """
        # Линейная скорость (м/с -> см/с)
        linear_velocity = msg.linear.x * 100.0
        # Угловая скорость (рад/с)
        angular_velocity = msg.angular.z

        # Дифференциальная кинематика
        # V_left = V_linear - (omega * wheel_base / 2)
        # V_right = V_linear + (omega * wheel_base / 2)
        # wheel_base в метрах, поэтому результат формулы будет в м/с, затем конвертируем в см/с
        
        half_base = self.wheel_base / 2.0
        
        v_left_ms = linear_velocity / 100.0 - (angular_velocity * half_base)
        v_right_ms = linear_velocity / 100.0 + (angular_velocity * half_base)

        # Сохраняем в см/с для отправки на контроллер
        self.speed_left = int(v_left_ms * 100.0)
        self.speed_right = int(v_right_ms * 100.0)

    def send_commands(self):
        """Отправляет команды на Arduino, если соединение активно."""
        if self.serial_conn is None or not self.serial_conn.is_open:
            return

        # Команда только для моторов (серво удалены)
        # Формат: N <speed_left_cm_s> <speed_right_cm_s>
        motor_cmd = f"N {self.speed_left} {self.speed_right}\n"

        try:
            self.serial_conn.write(motor_cmd.encode())
        except Exception as e:
            self.get_logger().error(f'Serial write error: {e}')
            self.reconnect_serial()

    def read_serial(self):
        """Читает данные из последовательного порта, обрабатывает ошибки."""
        if self.serial_conn is None or not self.serial_conn.is_open:
            return

        try:
            while self.serial_conn.in_waiting > 0:
                try:
                    line = self.serial_conn.readline().decode('utf-8', errors='ignore').strip()
                    if line:
                        self.process_line(line)
                except UnicodeDecodeError:
                    self.get_logger().debug('Unicode decode error, skipping line')
                except Exception as e:
                    self.get_logger().error(f'Error reading line: {e}')
        except OSError as e:
            self.get_logger().error(f'Serial I/O error: {e}')
            self.reconnect_serial()
        except Exception as e:
            self.get_logger().error(f'Unexpected error in read_serial: {e}')
            self.reconnect_serial()

    def process_line(self, line):
        """Обрабатывает одну строку, полученную от Arduino."""
        if line.startswith('ENC:'):
            parts = line.split()
            if len(parts) == 3:
                try:
                    enc_left = int(parts[1])
                    enc_right = int(parts[2])
                    self.pub_enc_left.publish(Int32(data=enc_left))
                    self.pub_enc_right.publish(Int32(data=enc_right))
                    self.get_logger().debug(f'Published encoders: {enc_left}, {enc_right}')
                except ValueError:
                    self.get_logger().warn(f'Invalid encoder values: {parts[1:]}')
            else:
                self.get_logger().warn(f'Malformed ENC line: {line}')

    def destroy_node(self):
        """Закрывает последовательный порт при завершении узла."""
        if self.serial_conn and self.serial_conn.is_open:
            try:
                self.serial_conn.close()
            except:
                pass
        super().destroy_node()


def main(args=None):
    rclpy.init(args=args)
    node = ArduinoController()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == '__main__':
    main()