//
//  ContentView.swift
//  Cat
//
//  Created by 田元 on 2024/5/14.
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        VStack {
            VStack{
                HStack {
                    Text("My Home")
                        .font(.largeTitle)
                        .fontWeight(.bold)
                    Spacer()
                    Image(systemName: "plus")
                        .font(.title)
                }
                .padding()
                
                ScrollView(.horizontal, showsIndicators: false) {
                    HStack {
                        QuickView(ImageName:"battery.100",info:"Battery",status: "Good")
                        QuickView(ImageName:"cellularbars",info:"Signal",status: "Poor")
                        QuickView(ImageName:"airport.express",info:"Devices",status: "3")
                        Spacer()
                    }.padding()
                }
                
                HStack{
                    Text("Quick Play")
                        .font(.headline)
                        .padding()
                    Spacer()
                        
                }
                
                ScrollView(.horizontal, showsIndicators: false){
                    HStack {
                        QuickButton(ImageName: "house.fill",PlayName: "Trick or Treat")
                        QuickButton(ImageName: "house.fill",PlayName: "Where are you")
                        QuickButton(ImageName: "house.fill",PlayName: "Catch it!")
                        
                    }.padding()
                }
                
            }
                .background(LinearGradient(gradient: Gradient(colors: [Color("DarkPurple"), Color("LightPurple")]), startPoint: .top, endPoint: .bottom))
                .shadow(color: Color.black.opacity(0.25), radius: 4, x: 0, y: 4)
                
                
                
            HStack{
                Text("Devices")
                    .font(.headline)
                    .padding()
                Spacer()
            }
            
            ScrollView{
                VStack{
                    DeviceView(name: "A Big Toy", count: 3)
                    DeviceView(name: "A TOY", count: 3)
                    DeviceView(name: "A TOY", count: 3)
                }.padding()
            }


         

        
        
        
        TabView {
            Text("")
                .tabItem {
                    Image(systemName: "house")
                    Text("Home")
                }
            
            Text("")
                .tabItem {
                    Image(systemName: "magnifyingglass")
                    Text("Discover")
                }
            
            Text("")
                .tabItem {
                    Image(systemName: "person")
                    Text("My")
                }
        }
        .frame(height: 50)
        }
    }
}

struct QuickView: View {
    let ImageName: String
    let info: String
    let status: String
    
    var body: some View {

        HStack {
            Image(systemName: ImageName)
                .font(.system(size: 12))
            VStack(alignment: .leading){
                Text(info)
                    .font(.system(size: 12))
                Text(status)
                    .font(.system(size: 12))
                    
            }
            
            
        }
        .fixedSize(horizontal: true, vertical: false)
        .frame(height: 10)
        .padding()
        .background(Color("LightPurple"))
        .cornerRadius(20)
        .shadow(color: Color.black.opacity(0.25), radius: 4, x: 4, y: 4)
        .shadow(color: Color.white.opacity(0.25), radius: 4, x: -1, y: -1)
        
    }
}

struct QuickButton: View {
    let ImageName: String
    let PlayName: String
    
    var body: some View {
        Button(action: {
            // Handle button action
        }) {
            HStack{
                Image(systemName: ImageName)
                    .font(.title)
                    .font(.subheadline)
                Text(PlayName)
                    .font(.system(size: 16))
                    .frame(alignment: .leading)
                
                    
            }
            .padding()
            .foregroundStyle(.black)
            .fixedSize(horizontal: true, vertical: false)
            .frame(width: 170)
            .background(Color("LightPurple"))
            .cornerRadius(10)
            .shadow(color: Color.black.opacity(0.25), radius: 4, x: 4, y: 4)
            .shadow(color: Color.white.opacity(0.35), radius: 4, x: -1, y: -1)
        }
    }
}

struct DeviceView: View {
    let name: String
    let count: Int
    
    var body: some View {
        HStack {
            Rectangle()
                .fill(Color.gray.opacity(0.2))
                .frame(width: 150, height: 150)
            
            Spacer()
            
            VStack(alignment: .trailing) {
                Text(name)
                    .font(.subheadline)
                Text("\(count) Times")
                    .font(.caption)
                Divider()
                    .frame(height: 30)
                HStack{
                    Button(action: {
                                            // Handle first button action
                    }) {
                        Image(systemName: "lightbulb.fill")
                            .font(.system(size: 20))
                            .padding(8)
                            .background(Color.black.opacity(0.25))
                            .foregroundColor(.white)
                            .clipShape(Circle())
                        
                    }
                        .shadow(color: Color.black.opacity(0.25), radius: 4, x: 4, y: 4)
                        .shadow(color: Color.white.opacity(0.35), radius: 4, x: -1, y: -1)
                    
                    Button(action: {
                        // Handle second button action
                    }) {
                        Image(systemName: "music.quarternote.3")
                            .font(.system(size: 18))
                            .padding(8)
                            .background(Color.black.opacity(0.25))
                            .foregroundColor(.white)
                            .clipShape(Circle())
                    }
                        .shadow(color: Color.black.opacity(0.25), radius: 4, x: 4, y: 4)
                        .shadow(color: Color.white.opacity(0.35), radius: 4, x: -1, y: -1)
                    
                    Button(action: {
                        // Handle third button action
                    }) {
                        Image(systemName:"waveform.path")
                            .font(.system(size: 15))
                            .padding(8)
                            .background(Color.black.opacity(0.25))
                            .foregroundColor(.white)
                            .clipShape(Circle())
                    }
                        .shadow(color: Color.black.opacity(0.25), radius: 4, x: 4, y: 4)
                        .shadow(color: Color.white.opacity(0.35), radius: 4, x: -1, y: -1)
                }
            }
            .alignmentGuide(.trailing) { d in d[.trailing] }
            
        
            
        
        }
        .padding(.horizontal)
        .background(Color.gray.opacity(0.2))
        .cornerRadius(10)
        .shadow(color: Color.gray.opacity(0.5), radius: 4, x: 4, y: 4)
        .shadow(color: Color.white.opacity(0.5), radius: 4, x: -1, y: -1)
    }
}

#Preview {
    ContentView()
}
