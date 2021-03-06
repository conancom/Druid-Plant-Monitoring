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
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
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
import org.jetbrains.anko.textColor
import org.jetbrains.anko.yesButton
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale
import java.util.UUID

class BleOperationsActivity : AppCompatActivity() {

    private lateinit var textTemp:TextView
    private lateinit var textSoil:TextView
    private lateinit var textAir:TextView
    private lateinit var textHumidity:TextView
    private lateinit var textBrightness:TextView
    private lateinit var togButton:Button
    private lateinit var mainChar:BluetoothGattCharacteristic


/*
    val textTemp:TextView = TODO() //findViewById<TextView>(R.id.textTemp)
    val textSoil:TextView = TODO() //findViewById<TextView>(R.id.textSoil)
    val textAir:TextView = TODO()//findViewById<TextView>(R.id.textAriQu)
    val textHumidity:TextView = TODO()//findViewById<TextView>(R.id.textHumidity)
    val textBrightness:TextView = TODO()//findViewById<TextView>(R.id.textBrightness)
*/

    private lateinit var device: BluetoothDevice


    private val dateFormatter = SimpleDateFormat("MMM d, HH:mm:ss", Locale.US)
    private val characteristics by lazy {
        ConnectionManager.servicesOnDevice(device)?.flatMap { service ->

                service.characteristics ?: listOf()


        } ?: listOf()
    }
    private val characteristicProperties by lazy {
        characteristics.map { characteristic ->
            characteristic to mutableListOf<CharacteristicProperty>().apply {
                if (characteristic.isNotifiable()) add(CharacteristicProperty.Notifiable)
                if (characteristic.isIndicatable()) add(CharacteristicProperty.Indicatable)
                if (characteristic.isReadable()) add(CharacteristicProperty.Readable)
                if (characteristic.isWritable()) add(CharacteristicProperty.Writable)
                if (characteristic.isWritableWithoutResponse()) {
                    add(CharacteristicProperty.WritableWithoutResponse)
                }
            }.toList()
        }.toMap()
    }
    private val characteristicAdapter: CharacteristicAdapter by lazy {
        CharacteristicAdapter(characteristics) { characteristic ->

                showCharacteristicOptions(characteristic)

        }
    }
    private var notifyingCharacteristics = mutableListOf<UUID>()

    override fun onCreate(savedInstanceState: Bundle?) {
        ConnectionManager.registerListener(connectionEventListener)
        super.onCreate(savedInstanceState)
        device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE)
            ?: error("Missing BluetoothDevice from MainActivity!")

        setContentView(R.layout.activity_ble_operations)
        supportActionBar?.apply {
            setDisplayHomeAsUpEnabled(true)
            setDisplayShowTitleEnabled(true)
            title = getString(R.string.ble_playground)
        }
        setupRecyclerView()

    }

    override fun onDestroy() {
        ConnectionManager.unregisterListener(connectionEventListener)
        ConnectionManager.teardownConnection(device)
        super.onDestroy()
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when (item.itemId) {
            android.R.id.home -> {
                onBackPressed()
                return true
            }
        }
        return super.onOptionsItemSelected(item)
    }

    private fun setupRecyclerView() {
        characteristics_recycler_view.apply {
            adapter = characteristicAdapter
            layoutManager = LinearLayoutManager(
                this@BleOperationsActivity,
                RecyclerView.VERTICAL,
                false

            )
            isNestedScrollingEnabled = false
        }

        val animator = characteristics_recycler_view.itemAnimator
        if (animator is SimpleItemAnimator) {
            animator.supportsChangeAnimations = false
        }
    }

    @SuppressLint("SetTextI18n")
    private fun log(message: String) {
        val formattedMessage = String.format("%s: %s", dateFormatter.format(Date()), message)
        runOnUiThread {
            val currentLogText = if (log_text_view.text.isEmpty()) {
                "Beginning of log."
            } else {
                log_text_view.text
            }
            log_text_view.text = "$currentLogText\n$formattedMessage"
            log_scroll_view.post { log_scroll_view.fullScroll(View.FOCUS_DOWN) }
        }
    }

    private fun showCharacteristicOptions(characteristic: BluetoothGattCharacteristic) {

        characteristicProperties[characteristic]?.let { properties ->
            selector("Select an action to perform", properties.map { it.action }) { _, i ->
                when (properties[i]) {
                    CharacteristicProperty.Readable -> {
                        log("Reading from ${characteristic.uuid}")
                        ConnectionManager.readCharacteristic(device, characteristic)
                    }
                    CharacteristicProperty.Writable, CharacteristicProperty.WritableWithoutResponse -> {
                        showWritePayloadDialog(characteristic)
                    }
                    CharacteristicProperty.Notifiable, CharacteristicProperty.Indicatable -> {
                        if (notifyingCharacteristics.contains(characteristic.uuid)) {
                            log("Disabling notifications on ${characteristic.uuid}")
                            ConnectionManager.disableNotifications(device, characteristic)
                        } else {
                            log("Enabling notifications on ${characteristic.uuid}")
                            ConnectionManager.enableNotifications(device, characteristic)

                            //val ln = findViewById<LinearLayout>(R.id.linlayout)
                            //ln.visibility = View.INVISIBLE

                            //val lnmain = findViewById<LinearLayout>(R.id.layMain)
                            //lnmain.visibility = View.VISIBLE



                            /*
                            val intent = Intent(this@BleOperationsActivity, BleInformationActivityActivity::class.java).also {
                                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                                    it.putExtra("Characteristic", characteristic)
                                }
                            }.apply {  }
                            startActivity(intent)

                             */

                            setContentView(R.layout.activity_main_information)
                            textTemp = findViewById(R.id.textTemp)
                            textSoil = findViewById(R.id.textSoil)
                            textAir = findViewById(R.id.textAriQu)
                            textHumidity = findViewById(R.id.textHumidity)
                            textBrightness = findViewById(R.id.textBrightness)
                            togButton = findViewById(R.id.tbutton)
                            togButton.setOnClickListener {
                                ConnectionManager.writeCharacteristic(device, characteristic, "1".toByteArray())
                            }
                        }
                    }
                }
            }
        }
    }

    @OptIn(ExperimentalStdlibApi::class)
    @SuppressLint("InflateParams")
    private fun showWritePayloadDialog(characteristic: BluetoothGattCharacteristic) {
        val hexField = layoutInflater.inflate(R.layout.edittext_hex_payload, null) as EditText
        alert {
            customView = hexField
            isCancelable = false
            yesButton {
                with(hexField.text.toString()) {
                    if (isNotBlank() && isNotEmpty()) {
                        val bytes = toByteArray()
                        log("Writing to ${characteristic.uuid}: ${bytes.decodeToString()}")
                        ConnectionManager.writeCharacteristic(device, characteristic, bytes)
                        //000ffe1-0000-1000-8000-00805f9b34fb
                        //ConnectionManager.writeCharacteristic(device
                    } else {
                        log("Please enter a hex payload to write to ${characteristic.uuid}")
                    }
                }
            }
            noButton {}
        }.show()
        hexField.showKeyboard()
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

            onCharacteristicRead = { _, characteristic ->
                var temp = characteristic.value.decodeToString().split(";").get(0);
                var soil_moisture = characteristic.value.decodeToString().split(";").get(1);
                var air_quality = characteristic.value.decodeToString().split(";").get(2);
                var humidity = characteristic.value.decodeToString().split(";").get(3);
                var brightness = characteristic.value.decodeToString().split(";").get(4);
                log("Read from ${characteristic.uuid}:")
                log("Temperature ${characteristic.uuid}: ${temp}")
                log("Soil Moisture ${characteristic.uuid}: ${soil_moisture}")
                log("Air Quality ${characteristic.uuid}: ${air_quality}")
                log("Humidity ${characteristic.uuid}: ${humidity}")
                log("Brightness ${characteristic.uuid}: ${brightness}")
            }

            onCharacteristicWrite = { _, characteristic ->
                log("Wrote to ${characteristic.uuid}")
            }

            onMtuChanged = { _, mtu ->
                log("MTU updated to $mtu")
            }

            onCharacteristicChanged = { _, characteristic ->
                log("Value changed on ${characteristic.uuid}: ${characteristic.value.toHexString()}")




                var temp = characteristic.value.decodeToString().split(";").get(0);
                var soil_moisture = characteristic.value.decodeToString().split(";").get(1);
                var air_quality = characteristic.value.decodeToString().split(";").get(2);
                var humidity = characteristic.value.decodeToString().split(";").get(3);
                var brightness = characteristic.value.decodeToString().split(";").get(4);


                if (air_quality.toInt() == 0) {
                    air_quality = "Good"
                    textAir.textColor = (-0xFF00FF)
                } else if (air_quality.toInt() == 1){
                    air_quality = "Moderate"
                    textAir.textColor = (-0xFF00FF)
                } else if (air_quality.toInt() == 2){
                    air_quality = "Poor"
                    textAir.textColor = (-0x00FF00)
                } else {
                    air_quality = "Very Poor"
                    textAir.textColor = (-0x00FF00)
                }


                if (soil_moisture.toInt() == 0) {
                    soil_moisture = "Dry"
                } else  {
                    soil_moisture = "Wet"
                }

                textTemp.text = ("${temp}??C" )
                textSoil.text = ("${soil_moisture}")
                textAir.text =("${air_quality}")
                textHumidity.text = ("${humidity}%")
                textBrightness.text = ("${brightness} Lumens")


            }

            onNotificationsEnabled = { _, characteristic ->
                log("Enabled notifications on ${characteristic.uuid}")
                notifyingCharacteristics.add(characteristic.uuid)

            }

            onNotificationsDisabled = { _, characteristic ->
                log("Disabled notifications on ${characteristic.uuid}")
                notifyingCharacteristics.remove(characteristic.uuid)
            }
        }
    }

    private enum class CharacteristicProperty {
        Readable,
        Writable,
        WritableWithoutResponse,
        Notifiable,
        Indicatable;

        val action
            get() = when (this) {
                Readable -> "Read"
                Writable -> "Write"
                WritableWithoutResponse -> "Write Without Response"
                Notifiable -> "Toggle Notifications"
                Indicatable -> "Toggle Indications"
            }
    }

    private fun Activity.hideKeyboard() {
        hideKeyboard(currentFocus ?: View(this))
    }

    private fun Context.hideKeyboard(view: View) {
        val inputMethodManager = getSystemService(Activity.INPUT_METHOD_SERVICE) as InputMethodManager
        inputMethodManager.hideSoftInputFromWindow(view.windowToken, 0)
    }

    private fun EditText.showKeyboard() {
        val inputMethodManager = getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager
        requestFocus()
        inputMethodManager.showSoftInput(this, InputMethodManager.SHOW_IMPLICIT)
    }

    private fun String.hexToBytes() =
        this.chunked(2).map { it.toUpperCase(Locale.US).toInt(16).toByte() }.toByteArray()
}
    private fun String.toAscii() = this.map { it.toInt() }.joinToString()
