import 'package:flutter/material.dart';

class LoginMyButton extends StatelessWidget {
  final Function()? onTap;

  const LoginMyButton({super.key, required this.onTap});

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        padding: const EdgeInsets.all(25),
        decoration: BoxDecoration(
          color: Color.fromARGB(255, 71, 233, 133),
          borderRadius: BorderRadius.circular(8),
        ),
        child: const Center(
          child: Text(
            "Continue",
            style: TextStyle(
                color: Colors.white, fontWeight: FontWeight.bold, fontSize: 25),
          ),
        ),
      ),
    );
  }
}

class LoginMyButtonAgree extends StatelessWidget {
  final Function()? onTap;
  final String text;
  const LoginMyButtonAgree(
      {super.key, required this.onTap, required this.text});

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        padding: const EdgeInsets.all(25),
        decoration: BoxDecoration(
          color: const Color.fromARGB(255, 71, 233, 133),
          borderRadius: BorderRadius.circular(8),
        ),
        child: Center(
          child: Text(
            text,
            style: const TextStyle(
                color: Colors.white, fontWeight: FontWeight.bold, fontSize: 25),
          ),
        ),
      ),
    );
  }
}
