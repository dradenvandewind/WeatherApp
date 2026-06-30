output "public_ip" {
  description = "Public IP of the instance"
  value       = aws_instance.this.public_ip
}

output "ssh_command" {
  description = "Command to connect via SSH"
  value       = "ssh -i ~/.ssh/${var.key_name}.pem ubuntu@${aws_instance.this.public_ip}"
}

output "app_url" {
  description = "Application URL (via Nginx)"
  value       = "http://${aws_instance.this.public_ip}:8080"
}

output "grafana_url" {
  description = "Grafana URL (admin/admin)"
  value       = "http://${aws_instance.this.public_ip}:3000"
}

output "prometheus_url" {
  description = "Prometheus URL"
  value       = "http://${aws_instance.this.public_ip}:9090"
}


# Automatically generate the Ansible inventory with the created machine's IP
resource "local_file" "ansible_inventory" {
  filename = "${path.module}/../ansible/inventory.ini"
  content  = <<-EOT
    [weather_app]
    ${aws_instance.this.public_ip} ansible_user=ubuntu ansible_ssh_private_key_file=${pathexpand("~/.ssh/${var.key_name}.pem")} ansible_ssh_common_args='-o StrictHostKeyChecking=no'
  EOT
}
