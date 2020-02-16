import React, { Component } from 'react';
import { FlatGrid } from 'react-native-super-grid';
import {
  Platform,
  StyleSheet,
  Text,
  View,
  Button,
  FlatList,
  Switch,
  TouchableOpacity,
  ToastAndroid,
  ImageBackground,
  Alert
} from 'react-native';

var _ = require('lodash');
import BluetoothSerial from 'react-native-bluetooth-serial'

export default class App extends Component {
    constructor (props) {
        super(props)
        this.state = {
            isEnabled: false,
            discovering: false,
            devices: [],
            unpairedDevices: [],
            connected: false,
            showBluetoothDevices: false,
            selectedCell: 100,
            currentCell: 16
        }
    }

    componentWillMount(){

        Promise.all([
            BluetoothSerial.isEnabled(),
            BluetoothSerial.list()
        ])
        .then((values) => {
            const [ isEnabled, devices ] = values

            this.setState({ isEnabled, devices })
        })

        BluetoothSerial.on('bluetoothEnabled', () => {

            Promise.all([
                BluetoothSerial.isEnabled(),
                BluetoothSerial.list()
            ])
            .then((values) => {
                const [ isEnabled, devices ] = values
                this.setState({  devices })
            })

            BluetoothSerial.on('bluetoothDisabled', () => {

                this.setState({ devices: [] })

            })
            BluetoothSerial.on('error', (err) => console.log(`Error: ${err.message}`))

        })
    }

    connect (device) {
        this.setState({ connecting: true })
        BluetoothSerial.connect(device.id)
        .then((res) => {
            console.log(`Connected to device ${device.name}`);
            this.setState({connected: true})
            ToastAndroid.show(`Connected to device ${device.name}`, ToastAndroid.SHORT);
        })
        .catch((err) => {
            console.log((err.message))
            this.setState({connected: false})
        })
    }

    _renderItem(item){
        return(<TouchableOpacity onPress={() => this.connect(item.item)}>
            <View style={styles.deviceNameWrap}>
                <Text style={styles.deviceName}>{ item.item.name ? item.item.name : item.item.id }</Text>
            </View>
            </TouchableOpacity>)
    }
    enable () {
        BluetoothSerial.enable()
        .then((res) => this.setState({ isEnabled: true }))
        .catch((err) => Toast.showShortBottom(err.message))
    }

    disable () {
        BluetoothSerial.disable()
        .then((res) => this.setState({ isEnabled: false }))
        .catch((err) => Toast.showShortBottom(err.message))
    }

    toggleBluetooth (value) {
        if (value === true) {
            this.enable()
        } else {
            this.disable()
        }
    }
    discoverAvailableDevices () {
        if (this.state.discovering) {
            return false
        } else {
            this.setState({ discovering: true })
            BluetoothSerial.discoverUnpairedDevices()
            .then((unpairedDevices) => {
                const uniqueDevices = _.uniqBy(unpairedDevices, 'id');
                console.log(uniqueDevices);
                this.setState({ unpairedDevices: uniqueDevices, discovering: false })
            })
            .catch((err) => console.log(err.message))
        }
    }
    toggleSwitch(cellChar, cellNumber) {
        if (this.state.selectedCell == cellNumber) {
            Alert.alert("You've already selected this destination.")
        } else if (!this.state.connected) {
            Alert.alert("You need to connect to a device first.")
        } else {
            this.setState({selectedCell: cellNumber})
            BluetoothSerial.write(cellChar)
            .then((res) => {
                console.log(res);
                console.log('Successfuly wrote to device')
                this.setState({ connected: true })
            })
            .catch((err) => console.log(err.message))
        }
    }
    toggleShowDevices(){
        this.setState({showBluetoothDevices: !this.state.showBluetoothDevices})
    }
    reachDestination() {
        if (this.state.selectedCell !== 100) {
            var destination = this.state.selectedCell
            this.setState({selectedCell: 100, currentCell: destination})
            BluetoothSerial.write('z')
            .then((res) => {
                console.log("Successfuly wrote " + "'z'" + " to device")
                    this.setState({ connected: true })
                })
            .catch((err) => console.log(err.message))
            Alert.alert("You've reached your destination! Switching lights off...")
        } else {
            Alert.alert("You haven't picked a destination yet.")
        }
    }
    render() {
        return (
            <View style={styles.container}>
            <Button
                title={this.state.showBluetoothDevices ? "Hide Available Devices" : "Show Available Devices"}
                color="#534B62"
                onPress={() => this.toggleShowDevices()}
            />
            {this.state.showBluetoothDevices &&
            <>
            <View style={styles.toolbar} >
                <Text style={styles.toolbarTitle}>Bluetooth Device List</Text>
                <View style={styles.toolbarButton}>

                <Switch
                    value={this.state.isEnabled}
                    onValueChange={(val) => this.toggleBluetooth(val)}
                />
            </View>
            </View>
            <Button
                onPress={this.discoverAvailableDevices.bind(this)}
                title="Scan for Devices"
                color="#D0BCD5"
            />
            <FlatList
                style={{flex:1}}
                data={this.state.devices}
                keyExtractor={item => item.id}
                renderItem={(item) => this._renderItem(item)}
            />
            </>
            }
            {!this.state.showBluetoothDevices &&
            <View style={{flex: 1}}>
            <ImageBackground source={require("./warehouse_topview.png")} style={{width: '100%', height: '100%'}}>
            <FlatGrid
                items={['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r']}
                itemDimension={80}
                renderItem={({ item, index }) => (                    
                    <TouchableOpacity onPress={() => this.toggleSwitch(item, index)}>
                        <View style={((this.state.selectedCell === index) && styles.selectedGridItem) || ((this.state.currentCell === index) && styles.currentGridItem) || styles.gridItem}>
                            {(this.state.currentCell === index) && <Text style={styles.hereText}>
                                You are here
                            </Text>}
                        </View>
                    </TouchableOpacity>
                )}
            />
            {(this.state.selectedCell !== 100) && <Button
                onPress={this.reachDestination.bind(this)}
                title="Reached destination!"
                color="#BEEDAA"
                style={styles.reachButton}
            />}
            </ImageBackground>
            </View>}
            </View>
        );
    }
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        backgroundColor: '#F5FCFF',
        fontFamily: 'Roboto'
    },
    toolbar:{
        paddingTop:30,
        paddingBottom:30,
        flexDirection:'row'
    },
    toolbarButton:{
        width: 50,
        marginTop: 8,
    },
    toolbarTitle:{
        textAlign:'center',
        fontWeight:'bold',
        fontFamily: 'Cochin',
        fontSize: 20,
        flex:1,
        marginTop:6
    },
    deviceName: {
        fontSize: 17,
        color: "black",
        fontFamily: 'Roboto'
    },
    deviceNameWrap: {
        margin: 10,
        borderBottomWidth:1
    },
    gridItem: {
        padding: 30,
        margin: 10,
        textAlign: "center",
        backgroundColor: '#A499B3',
        opacity: 0.3
    },
    selectedGridItem: {
        padding: 30,
        margin: 10,
        textAlign: "center",
        backgroundColor: '#BEEDAA',
        opacity: 0.5
    },
    currentGridItem: {
        paddingHorizontal: 20,
        paddingVertical: 12,
        margin: 10,
        backgroundColor: '#ED474A',
        opacity: 0.5
    },
    hereText: {
        textAlign: "center"
    }
});