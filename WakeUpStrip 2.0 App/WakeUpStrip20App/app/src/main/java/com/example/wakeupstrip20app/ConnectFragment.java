package com.example.wakeupstrip20app;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import java.util.ArrayList;
import java.util.Set;
import static android.R.layout.simple_list_item_1;

public class ConnectFragment extends Fragment {
    private TextView textInfo;
    ListView listViewPairedDevice;
    Button bSetup;
    ArrayList<String> pairedDeviceArrayList;
    ArrayAdapter pairedDeviceAdapter;

    @Nullable
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_connect, container, false);
        textInfo = v.findViewById(R.id.textInfoPhone);
        bSetup = v.findViewById(R.id.butSetup);
        bSetup.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Set<BluetoothDevice> pairedDevices = MainActivity.bluetoothAdapter.getBondedDevices();
                if (pairedDevices.size() > 0) {
                    pairedDeviceArrayList = new ArrayList<>();
                    for (BluetoothDevice device : pairedDevices) {
                        pairedDeviceArrayList.add(device.getName() + "\n" + device.getAddress());
                    }
                    pairedDeviceAdapter = new ArrayAdapter<>(inflater.getContext(), simple_list_item_1, pairedDeviceArrayList);
                    listViewPairedDevice.setAdapter(pairedDeviceAdapter);
                }
            }
        });
        textInfo.setText(String.format("Это устройство: %s", MainActivity.bluetoothAdapter.getName() + " " + MainActivity.bluetoothAdapter.getAddress()));
        listViewPairedDevice = (ListView)v.findViewById(R.id.listDevices);
        return v;
    }


}
