import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:hydrolink/pages/login_page.dart';

class HomePage extends StatelessWidget {
  final FirebaseAuth _auth = FirebaseAuth.instance;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('HydroLink - Home'),
      ),
      body: Column(
        children: [
          Expanded(
            child: Container(
              color: Colors.blue,
              child: Center(
                child: Text(
                  'Welcome to HydroLink!',
                  style: TextStyle(
                    fontSize: 32.0,
                    color: Colors.white,
                  ),
                ),
              ),
            ),
          ),
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              ElevatedButton(
                onPressed: () async {
                  // Log out the user
                  await _auth.signOut();

                  // Navigate to the login page
                  Navigator.of(context).pushReplacement(
                    MaterialPageRoute(builder: (context) => LoginPage()),
                  );
                },
                child: Text('Log out'),
              ),
              SizedBox(width: 16.0),
              ElevatedButton(
                onPressed: () {
                  // TODO: Implement button action
                },
                child: Text('Button 2'),
              ),
            ],
          ),
        ],
      ),
    );
  }
}
