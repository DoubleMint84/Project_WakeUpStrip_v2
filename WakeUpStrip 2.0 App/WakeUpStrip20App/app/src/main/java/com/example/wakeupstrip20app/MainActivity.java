package com.example.wakeupstrip20app;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import com.google.android.material.bottomnavigation.BottomNavigationView;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.UUID;

import static android.R.layout.simple_list_item_1;

public class MainActivity extends AppCompatActivity {

    public static BluetoothAdapter bluetoothAdapter;
    public static ThreadConnectBTdevice myThreadConnectBTDevice;
    public static ThreadConnected myThreadConnected;
    public static UUID myUUID;
    public final String UUID_STRING_WELL_KNOWN_SPP = "00001101-0000-1000-8000-00805F9B34FB";
    private static final int REQUEST_ENABLE_BT = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        myThreadConnectBTDevice = new ThreadConnectBTdevice();
        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH)){
            Toast.makeText(this, "BLUETOOTH NOT support", Toast.LENGTH_LONG).show();
            //finish();
            return;
        }

        myUUID = UUID.fromString(UUID_STRING_WELL_KNOWN_SPP);
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (bluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth is not supported on this hardware platform", Toast.LENGTH_LONG).show();
            //finish();
            return;
        }
        BottomNavigationView bottomNav = findViewById(R.id.bottom_navigation);
        bottomNav.setOnNavigationItemSelectedListener(navListener);

        getSupportFragmentManager().beginTransaction().replace(R.id.fragment_container, new ConnectFragment()).commit();
    }

    @Override
    protected void onStart() {
        super.onStart();
        if (!bluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_ENABLE_BT) { // Если разрешили включить Bluetooth, тогда void setup()
            if (resultCode == Activity.RESULT_OK) {
                Toast.makeText(this, "BlueTooth включен, нажмите кнопку для открытия списка девайсов", Toast.LENGTH_SHORT).show();
            } else { // Если не разрешили, тогда закрываем приложение
                Toast.makeText(this, "BlueTooth не включён", Toast.LENGTH_SHORT).show();
                finish();
            }
        }
    }

    private BottomNavigationView.OnNavigationItemSelectedListener navListener = new BottomNavigationView.OnNavigationItemSelectedListener() {
        @Override
        public boolean onNavigationItemSelected(@NonNull MenuItem item) {
            Fragment selectedFragment = null;

            switch (item.getItemId()) {
                case R.id.nav_connection:
                    selectedFragment = new ConnectFragment();
                    break;
                case R.id.nav_alarms:
                    selectedFragment = new AlarmsFragment();
                    break;
                case R.id.nav_lamp:
                    selectedFragment = new LampFragment();
                    break;
                case R.id.nav_device_settings:
                    selectedFragment = new DeviceSettingsFragment();
                    break;
                case R.id.nav_app_settings:
                    selectedFragment = new AppSettingsFragment();
                    break;
            }

            getSupportFragmentManager().beginTransaction().replace(R.id.fragment_container, selectedFragment).commit();
            return true;
        }
    };

    public class ThreadConnectBTdevice extends Thread { // Поток для коннекта с Bluetooth
        private BluetoothSocket bluetoothSocket = null;
        ThreadConnectBTdevice() {

        }

        public void setSocket(BluetoothDevice device) {
            try {
                bluetoothSocket = device.createRfcommSocketToServiceRecord(myUUID);
            }
            catch (IOException e) {
                e.printStackTrace();
            }
        }


        @Override
        public void run() { // Коннект
            boolean success = false;
            try {
                bluetoothSocket.connect();
                success = true;
            }
            catch (IOException e) {
                e.printStackTrace();
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(getApplicationContext(), "Нет коннекта, проверьте Bluetooth-устройство с которым хотите соединица!", Toast.LENGTH_LONG).show();
                        Fragment conFrag = getSupportFragmentManager().findFragmentById(R.id.fragment_container);
                        ((ListView) conFrag.getView().findViewById(R.id.listDevices)).setEnabled(true);
                    }
                });

                //ConnectFragment.listViewPairedDevice.setVisibility(View.VISIBLE);
                try {
                    bluetoothSocket.close();
                }
                catch (IOException e1) {
                    e1.printStackTrace();
                }
            }
            if(success) {  // Если законнектились, тогда открываем панель с кнопками и запускаем поток приёма и отправки данных
                runOnUiThread(new Runnable() {

                    @Override
                    public void run() {
                        Toast.makeText(getApplicationContext(), "Connected", Toast.LENGTH_LONG).show();
                        //ButPanel.setVisibility(View.VISIBLE); // открываем панель с кнопками
                    }
                });

                myThreadConnected = new ThreadConnected(bluetoothSocket);
                myThreadConnected.start(); // запуск потока приёма и отправки данных
            }
        }

        public void cancel() {
            Toast.makeText(getApplicationContext(), "Close - BluetoothSocket", Toast.LENGTH_LONG).show();
            try {
                bluetoothSocket.close();
            }
            catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (myThreadConnectBTDevice !=null) myThreadConnectBTDevice.cancel();
    }

    private class ThreadConnected extends Thread {    // Поток - приём и отправка данных
        private final InputStream connectedInputStream;
        private final OutputStream connectedOutputStream;
        private String sbprint;
        public ThreadConnected(BluetoothSocket socket) {
            InputStream in = null;
            OutputStream out = null;
            try {
                in = socket.getInputStream();
                out = socket.getOutputStream();
            }
            catch (IOException e) {
                e.printStackTrace();
            }
            connectedInputStream = in;
            connectedOutputStream = out;
        }

        @Override
        public void run() { // Приём данных
            /* while (true) {
                try {
                    byte[] buffer = new byte[1];
                    int bytes = connectedInputStream.read(buffer);
                    String strIncom = new String(buffer, 0, bytes);
                    sb.append(strIncom); // собираем символы в строку
                    int endOfLineIndex = sb.indexOf("\r\n"); // определяем конец строки
                    if (endOfLineIndex > 0) {
                        sbprint = sb.substring(0, endOfLineIndex);
                        sb.delete(0, sb.length());
                        runOnUiThread(new Runnable() { // Вывод данных

                            @Override
                            public void run() {
                                switch (sbprint) {

                                    case "D10 ON":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D10 OFF":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D11 ON":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D11 OFF":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D12 ON":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D12 OFF":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D13 ON":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    case "D13 OFF":
                                        Toast.makeText(MainActivity.this, sbprint, Toast.LENGTH_SHORT).show();
                                        break;

                                    default:
                                        break;
                                }
                            }
                        });
                    }
                } catch (IOException e) {
                    break;
                }
            } */
        }


        public void write(byte[] buffer) {
            try {
                connectedOutputStream.write(buffer);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }
}
