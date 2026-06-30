variable "aws_region" {
  description = "AWS region"
  type        = string
  default     = "eu-west-3" # Paris
}

variable "instance_type" {
  description = "EC2 instance type"
  type        = string
  default     = "t3.medium" # docker-compose + prometheus + grafana + cadvisor : plan for at least 4 GB RAM
}

variable "project_name" {
  description = "Prefix used to name resources"
  type        = string
  default     = "weather-app"
}

variable "ssh_allowed_cidr" {
  description = "CIDR allowed for SSH (set your IP/32 in production)"
  type        = string
  default     = "0.0.0.0/0"
}

variable "key_name" {
  description = "Name of the SSH key pair created on AWS"
  type        = string
  default     = "weather-app-key"
}
