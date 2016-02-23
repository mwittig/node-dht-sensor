{
  'targets': [
    {
      "target_name": "node_dht_sensor",
      "sources": [ "node-dht-sensor.cpp" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "libraries": [ "-lwiringPi" ],
      "conditions": [ 
        ["OS=='linux'", {
          "sources": ["node-dht-sensor.cpp"] 
        }]
      ]
    }
  ]
}
