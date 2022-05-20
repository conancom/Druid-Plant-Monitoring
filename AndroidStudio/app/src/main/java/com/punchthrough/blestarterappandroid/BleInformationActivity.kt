/*
 * Copyright 2019 Punch Through Design LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.punchthrough.blestarterappandroid

import android.annotation.SuppressLint
import android.app.Activity
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothGattCharacteristic
import android.content.Context
import android.os.Bundle
import android.view.MenuItem
import android.view.View
import android.view.inputmethod.InputMethodManager
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.isVisible
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import androidx.recyclerview.widget.SimpleItemAnimator
import com.punchthrough.blestarterappandroid.ble.ConnectionEventListener
import com.punchthrough.blestarterappandroid.ble.ConnectionManager
import com.punchthrough.blestarterappandroid.ble.isIndicatable
import com.punchthrough.blestarterappandroid.ble.isNotifiable
import com.punchthrough.blestarterappandroid.ble.isReadable
import com.punchthrough.blestarterappandroid.ble.isWritable
import com.punchthrough.blestarterappandroid.ble.isWritableWithoutResponse
import com.punchthrough.blestarterappandroid.ble.toHexString
import kotlinx.android.synthetic.main.activity_ble_operations.characteristics_recycler_view
import kotlinx.android.synthetic.main.activity_ble_operations.log_scroll_view
import kotlinx.android.synthetic.main.activity_ble_operations.log_text_view
import org.jetbrains.anko.alert
import org.jetbrains.anko.noButton
import org.jetbrains.anko.selector
import org.jetbrains.anko.yesButton
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import java.util.UUID

class BleInformationActivityActivity : AppCompatActivity() {

    private lateinit var textTemp: TextView
    private lateinit var textSoil: TextView
    private lateinit var textAir: TextView
    private lateinit var textHumidity: TextView
    private lateinit var textBrightness: TextView
    private lateinit var mainChar: BluetoothGattCharacteristic

    override fun onCreate(savedInstanceState: Bundle?) {
        ConnectionManager.registerListener(connectionEventListener)
        super.onCreate(savedInstanceState)

        mainChar = intent.getParcelableExtra("Characteristic")


        setContentView(R.layout.activity_main_information)
        supportActionBar?.apply {
            setDisplayHomeAsUpEnabled(true)
            setDisplayShowTitleEnabled(true)
            title = getString(R.string.ble_playground)


            textTemp = findViewById(R.id.textTemp)
            textSoil = findViewById(R.id.textSoil)
            textAir = findViewById(R.id.textAriQu)
            textHumidity = findViewById(R.id.textHumidity)
            textBrightness = findViewById(R.id.textBrightness)
        }


    }

    @OptIn(ExperimentalStdlibApi::class)
    private val connectionEventListener by lazy {
        ConnectionEventListener().apply {
            onDisconnect = {
                runOnUiThread {
                    alert {
                        title = "Disconnected"
                        message = "Disconnected from device."
                        positiveButton("OK") { onBackPressed() }
                    }.show()
                }
            }

            onCharacteristicChanged = { _, characteristic ->


                var temp = mainChar.value.decodeToString().split(";").get(0);
                var soil_moisture = mainChar.value.decodeToString().split(";").get(1);
                var air_quality = mainChar.value.decodeToString().split(";").get(2);
                var humidity = mainChar.value.decodeToString().split(";").get(3);
                var brightness = mainChar.value.decodeToString().split(";").get(4);




                textTemp.setText("Temperature: ${temp} C")
                textSoil.setText("Soil Moisture: ${soil_moisture} Moist")
                textAir.setText("Air Quality: ${air_quality} Good")
                textHumidity.setText("Humidity: ${humidity} Humid")
                textBrightness.setText("Brightness: ${brightness} Bright")


            }
        }


    }
}
